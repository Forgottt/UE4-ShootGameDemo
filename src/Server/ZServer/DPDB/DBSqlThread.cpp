#include "../MyZServerNetwork.h"
#include "DBSqlThread.h"
#include "../MyMsg.h"
#include "dbms.h"

void DBSqlThread::run()//子线程
{
	while (true)
	{
		std::vector<boost::shared_ptr<CWaitNode> > tmp;
		m_vecWriteLock.lock();
		tmp = m_vecWrite;
		m_vecWrite.clear();
		m_vecWriteLock.unlock();

		m_vecWriteFinishLock.lock();
		for (unsigned int i = 0; i < tmp.size(); i++)
		{
			tmp[i]->DoDBSql(&m_dbSqlThreadConnect);

			m_vecWriteFinish.push_back(tmp[i]);
		}
		m_vecWriteFinishLock.unlock();
		boost::thread::sleep(boost::get_system_time() + boost::posix_time::millisec(1));
	}
}

int CRegisterWaitNode::CheckAndInsertAccountAndPasswd(DBConnect * m_pthreaddb, const std::string  & strAccount, const std::string  & strPassword)
{
	//帐号合法性检测
	if (strAccount.length() > 20)
		return -1;

	std::string sql = "call p_addAccountByMyself('" + strAccount + "'" + " ,'" + strPassword + "',@accountid,@isNew);";
	//std::string sql = "selsect * frome zhenghuanjie_account where loginname = '" + Account + "';";
	MYSQL * tmpcon = m_pthreaddb->GetConn();
	if (tmpcon == NULL)
		return -2;

	//创建一个记录集
	boost::shared_ptr<DBresultSet> ret = boost::shared_ptr<DBresultSet>(new DBresultSet(NULL));

	//执行sql语句，并把数据放入记录集
	int iret = m_pthreaddb->loadtabledata(sql.c_str(), (DBresultSet *)ret.get());

	if (iret <= 0)
		return -3;

	MYSQL_RES *res = ret->mres;//将结果保存在res结构体中

	MYSQL_ROW  row;

	while (ret->mres != 0 && (row = mysql_fetch_row(res)) != 0)
	{
		try
		{
			int iAccountID = atoi(row[0]);
			int iIsNew = atoi(row[1]);
			if (iIsNew)
				return iAccountID;
			else
				return -4;//帐号重复
		}
		catch (std::exception &e)
		{
			std::string strErr = e.what();
			return -5;
		}
	}
	return -6;
}

void CRegisterWaitNode::DoDBSql(DBConnect * m_pthreaddb)
{
	m_outdata = CheckAndInsertAccountAndPasswd(m_pthreaddb,m_strAccount,m_strPassword);
}

void CRegisterWaitNode::DoLogic()
{
	ZS2C_A_RegisterAccount sendMsg;
	HMessage sendHmg;
	if (m_outdata > 0)
	{
		sendMsg.m_iErrorCode = ERROR_CODE_REGSTER_OK;
		sendMsg.m_iErrorCodeIndex = m_outdata;
		sendMsg.put(sendHmg);
	}
	else
	{
		sendMsg.m_iErrorCode = ERROR_CODE_REGSTER_FAIL;
		sendMsg.m_iErrorCodeIndex = m_outdata;
		sendMsg.put(sendHmg);
	}
	MyZServerNetwork::GetInstance()->SendMsgBySessionID(m_dwSessionID, &sendHmg);
}

bool CLoginWaitNode::CheckLoginData(DBConnect * m_pthreaddb, const std::string & Account, const std::string & password)
{
	std::string sql = "selsect * frome account where loginname = '" + Account + "' and password = '" + password + "';";
	MYSQL * tmpcon = m_pthreaddb->GetConn();
	if (tmpcon == NULL)
		return false;

	//创建一个记录集
	boost::shared_ptr<DBresultSet> ret = boost::shared_ptr<DBresultSet>(new DBresultSet(NULL));

	//执行sql语句，并把数据放入记录集
	int iret = m_pthreaddb->loadtabledata(sql.c_str(), (DBresultSet *)ret.get());

	if (iret <= 0)
		return false;

	MYSQL_RES *res = ret->mres;//将结果保存在res结构体中

	MYSQL_ROW  row;

	if (ret->mres != 0 && (row = mysql_fetch_row(res)) != 0) {
		try
		{
			int iFeildCount = mysql_field_count(tmpcon);
			if (iFeildCount == 1)
				return true;
		}
		catch (std::exception &e)
		{
			std::string strErr = e.what();
			std::cout << strErr << std::endl;
			return false;
		}
	}
	return false;
}


struct ServerData
{
	uint32_t dwID;
	std::string strName;
	std::string strIP;
	uint16_t    wPort;
	uint8_t     byState;
};
extern std::vector<ServerData > g_vecServerList;

int CLoginWaitNode::GetAllPerganaFromSql(DBConnect * m_pthreaddb)
{
	std::string sql = "selsect * frome gm_serverlist;";
	MYSQL * tmpcon = m_pthreaddb->GetConn();
	if (tmpcon == NULL)
		return 0;

	//创建一个记录集
	boost::shared_ptr<DBresultSet> ret = boost::shared_ptr<DBresultSet>(new DBresultSet(NULL));

	//执行sql语句，并把数据放入记录集
	int iret = m_pthreaddb->loadtabledata(sql.c_str(), (DBresultSet *)ret.get());

	if (iret <= 0)
		return 0;

	MYSQL_RES *res = ret->mres;//将结果保存在res结构体中

	MYSQL_ROW  row;

	int iFeildCount;

	while (ret->mres != 0 && (row = mysql_fetch_row(res)) != 0)
	{
		try
		{
			iFeildCount = mysql_field_count(tmpcon);
			if (iFeildCount <= 0)
				return 0;
			else
			{
				ServerData serverData;
				serverData.dwID = row[0] != nullptr ? atoi(row[0]) : 0;
				serverData.strName = row[1] != nullptr ? row[1] : "";
				serverData.strIP = row[2] != nullptr ? row[2] : "";
				serverData.wPort = row[3] != nullptr ? atoi(row[3]) : 0;
				serverData.byState = row[7] != nullptr ? atoi(row[7]) : 0;

				g_vecServerList.push_back(serverData);
			}
		}
		catch (std::exception &e)
		{
			std::string strErr = e.what();
			std::cout << strErr << std::endl;
			return 0;
		}
	}
	return iFeildCount;
}

void CLoginWaitNode::DoDBSql(DBConnect * m_pthreaddb)
{
	m_checkisOK = CheckLoginData(m_pthreaddb, m_strAccount, m_strPassword);
	m_servercount = GetAllPerganaFromSql(m_pthreaddb);
}

void CLoginWaitNode::DoLogic()
{
	ZS2C_A_Login sendMsg;
	HMessage sendHmg;
	if (true == m_checkisOK && m_servercount > 0)
	{
		//写入内存数据库
		std::string strValue = this->m_strAccount;
		DBMS::GetInstance()->Insert(this->m_strAccount, strValue, 10000);

		sendMsg.m_iErrorCode = ERROR_CODE_LOGIN_OK;
		sendMsg.m_DynmicCode = m_dwDynamicCode;
		sendMsg.put(sendHmg);

		for (unsigned int i = 0; i < m_servercount; ++i)
		{
			sendMsg.m_iID = g_vecServerList[i].dwID;
			sendMsg.m_strIP = g_vecServerList[i].strIP;
			sendMsg.m_wPort = g_vecServerList[i].wPort;
			sendMsg.m_State = g_vecServerList[i].byState;
			sendMsg.putServerData(sendHmg);
		}
	}
	else
	{
		sendMsg.m_iErrorCode = ERROR_CODE_LOGIN_FAIL;
		sendMsg.put(sendHmg);
	}
	MyZServerNetwork::GetInstance()->SendMsgBySessionID(m_dwSessionID, &sendHmg);
}

