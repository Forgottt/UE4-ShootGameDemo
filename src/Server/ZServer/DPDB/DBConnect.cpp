#include "DBConnect.h"
#include "string.h"
DBresultSet::DBresultSet(MYSQL_RES *res)
{
	mres = res;
}
DBresultSet::~DBresultSet()
{
	if (mres != NULL)
		 mysql_free_result(mres);//mysql_free_result(mres);
}

DBConnect::DBConnect()
{
    con = NULL;
}


DBConnect::~DBConnect()
{
    if (con != NULL)
    {
        mysql_close(con);
    }
}
int DBConnect::init(std::string strdbip, std::string dbname)
{
    std::string _dbuser= "daypet";
    std::string _dbpasswd = "daypet123456";

	int rt = 0;
	//con不为空表示已经连接
	if (con != NULL)
		return rt;
	//将传入的数据库ip名字记录下来
	m_strDBip = strdbip;
	m_strDBName = dbname;
	//database configuartion
	const char* dbuser = _dbuser.c_str();
	const char* dbpasswd = _dbpasswd.c_str();
	//printf("DB input over\n");
	//调用sql库本身的函数进行数据库初始化
	con = mysql_init((MYSQL*)0);
	//设置额外连接选项,并影响连接行为
#ifdef WIN32
	mysql_options(con, MYSQL_SET_CHARSET_NAME, "GBK");
#else
	mysql_options(con, MYSQL_SET_CHARSET_NAME, "utf8");
#endif
	//运行函数连接数据库
	if (con != NULL && mysql_real_connect(con, strdbip.c_str(), dbuser, dbpasswd, dbname.c_str(), 3306/*TCP IP端口*/, NULL/*Unix Socket 连接类型*/, 0/*运行成ODBC??数据库标志*/))
	{
		rt = 1;
		char value = 1;
		//设置断线重连
		mysql_options(con, MYSQL_OPT_RECONNECT, (char *)&value);
		if (!mysql_select_db(con, dbname.c_str()))//由db指定的数据库成为由mysql指定连接的默认数据库
		{
			//printf("Database Connect Success!\n");
			//设置断线重连，和上面的重复了
			con->reconnect = 1;
		}
	}
	else 
	{

		std::cout << "Unable to connect the database,check your configuration!";
		con = NULL;
	}
	return rt;
}
//手动重连，默认是用的自动重连
int DBConnect::KeepAlive(uint32_t timeMargin, const char * who)
{
	if (who == NULL || timeMargin == 0) {}
	{
		int err = mysql_ping(con);
		if (err)
		{
			Clear();
			int succ = init(m_strDBip, m_strDBName);
			return succ;
		}
	}
	return true;
}

int  DBConnect::loadtabledata(const char* sql, DBresultSet *pDataSet)
{
	if (con == 0 || 0 == KeepAlive(0, NULL))
		return -1;
	const std::string & strsql = sql;
	int rt;
	if (sql == NULL)
	{
		return -1;
	}
	rt = mysql_real_query(con, sql, strlen(sql));
	if (rt)
	{
		printf("Error making query: %s !!!\n", mysql_error(con));
		return -2;
	}
	if (sql[0] == 'c')
	{
		if (strsql.find("call") != std::string::npos)
		{
			std::string::size_type indexat = strsql.find("@");
			if (indexat == std::string::npos)
				return 1;
			int iEnd = strsql.length();
			std::string::size_type indexend = strsql.find(")");
			if (indexend != std::string::npos)
			{
				iEnd = indexend - indexat;
			}
			std::string callsql = "SELECT " + strsql.substr(indexat, iEnd);
			mysql_query(con, callsql.c_str());//"SELECT @ret,@ out_name,@ out_age ");
		}
	}
	if (sql[0] == 's' || sql[0] == 'c' || sql[0] == 'S' || sql[0] == 'C')
	{
		MYSQL_RES *res = mysql_store_result(con);//将结果保存在res结构体中
		if (res != 0)
		{
			pDataSet->mres = res;
			return 1;
		}
	}
	else
	{
		return 1;
	}
	return -3;
}
void DBConnect::Clear()
{
	if (con != NULL)
	{
		mysql_close(con);
		con = NULL;
	}
}
