#ifndef _DBCONNECT_H_
#define _DBCONNECT_H_
#include "mysql.h"
#include <iostream>
#include <stdint.h>
#include "stdio.h"
#pragma comment(lib, "libmysql.lib")
class DBresultSet
{
public:
	MYSQL_RES *mres;
	DBresultSet(MYSQL_RES *res);
	~DBresultSet();

};
class DBConnect
{

public:
	MYSQL * con; 
	MYSQL_RES *res;
	MYSQL_ROW row;
	MYSQL * GetConn() { return con; }
	std::string m_strDBip;
	std::string m_strDBName;
	DBConnect();
	~DBConnect();
        int init(std::string strdbip, std::string dbname);
	//手动重连，默认是用的自动重连
	int KeepAlive(uint32_t timeMargin, const char * who);
	int  loadtabledata(const char* sql, DBresultSet *pDataSet);
	void Clear();
};
#endif
