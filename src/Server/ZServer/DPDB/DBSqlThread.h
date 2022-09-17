#ifndef DBSQLTHREAD_H
#define DBSQLTHREAD_H
#include <boost/thread/mutex.hpp>
#include "DBConnect.h"
#include "boost/function.hpp"
#include <vector>

struct CWaitNode
{
	uint32_t m_dwSessionID;
	virtual void DoDBSql(DBConnect * m_pthreaddb) {}//执行sql语句 DB thread
	virtual void DoLogic() {}
	CWaitNode(){}
	virtual ~CWaitNode() {}
};

//注册
struct CRegisterWaitNode :public CWaitNode
{
	std::string m_strAccount;
	std::string m_strPassword;

	int m_outdata;
protected:
	int CheckAndInsertAccountAndPasswd(DBConnect * m_pthreaddb,const std::string  & strAccount, const std::string  & strPassword);//验证注册
public:
	CRegisterWaitNode(const std::string  & strAccount, const std::string  & strPassword)
	{
		m_strAccount = strAccount;
		m_strPassword = strPassword;
	}

	virtual void DoDBSql(DBConnect * m_pthreaddb);//执行sql语句 DB thread
	virtual void DoLogic();
};

struct CLoginWaitNode :public CWaitNode
{
	std::string m_strAccount;//in
	std::string m_strPassword;//in varchar(255)
	uint32_t m_dwDynamicCode;
	bool m_checkisOK;
	uint32_t m_servercount;//out
protected:
	//验证账号密码
	bool CheckLoginData(DBConnect * m_pthreaddb, const std::string & Account, const std::string & password);
	//获取大区服务器
	int GetAllPerganaFromSql(DBConnect * m_pthreaddb);
public:
	CLoginWaitNode(std::string strAccount, std::string strPassword, uint32_t dwDynamicCode)
	{
		m_dwDynamicCode = dwDynamicCode;
		m_strAccount = strAccount;
		m_strPassword = strPassword;
	}

	virtual void DoDBSql(DBConnect * m_pthreaddb);//执行sql语句 DB thread
	virtual void DoLogic();
};

class DBSqlThread//io_service  db_io_service
{
	std::vector<boost::shared_ptr<CWaitNode> > m_vecWrite;
	boost::mutex m_vecWriteLock;

	std::vector<boost::shared_ptr<CWaitNode> > m_vecWriteFinish;
	boost::mutex m_vecWriteFinishLock;
	DBConnect m_dbSqlThreadConnect;
	DBSqlThread() {}
public:
	static DBSqlThread *GetInstance()
	{
		static DBSqlThread *pOne = NULL;
		if (NULL == pOne)
		{
			pOne = new DBSqlThread;
			pOne->m_dbSqlThreadConnect.init("127.0.0.1", "daypet");
		}
		return pOne;
	}

	void AddWaitNode(boost::shared_ptr<CWaitNode>  pNode)//主线程 压入数据
	{
		m_vecWriteLock.lock();
		m_vecWrite.push_back(pNode);
		m_vecWriteLock.unlock();
	}

	void run();

	std::vector<boost::shared_ptr<CWaitNode> > GetFinishDBWait()//主线 取数据
	{
		std::vector<boost::shared_ptr<CWaitNode> > tmp;
		m_vecWriteFinishLock.lock();
		tmp = m_vecWriteFinish;
		m_vecWriteFinish.clear();
		m_vecWriteFinishLock.unlock();
		return tmp;
	}
};
#endif 
