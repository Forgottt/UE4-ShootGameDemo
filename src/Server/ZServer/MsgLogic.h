#pragma once
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "DPDB/DBConnect.h"
#include "DPDB/DBSqlThread.h"
extern DBConnect g_db2;
//前置声明消息类
class HMessage;
class SessionClient;
//消息处理类
class MsgLogic
{
	//消息列表（从接收的消息中获取）
	std::vector< boost::shared_ptr<HMessage> > m_vecAllClientMsg;
	std::map<int, boost::shared_ptr<SessionClient> > m_mapAllClient;

	std::vector< boost::shared_ptr< CWaitNode > > m_vecAllFinishWaitNode;
	//锁
public:
	static MsgLogic* GetInstance();
	void GetvecMsg(std::vector<boost::shared_ptr<HMessage>> vecmsg);
	void GetmapAllClient(std::map<int, boost::shared_ptr<SessionClient> > mapClient);
	void DealMsg();
	void DealSqlMsg();
	//动态码验证
	bool CheckDynmicCode(int id,int dc);
};
