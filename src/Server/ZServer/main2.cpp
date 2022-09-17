#include "MyZServerNetwork.h"
#include "MyMsg.h"
#include "MsgLogic.h"
#include "DPDB/DBSqlThread.h"
int  g_iShutDown = 0;

int MyZServerNetwork::g_iCurrentID = 0;

DBConnect g_db2;

int main()
{
	io_service my_io_service;
	MyZServerNetwork::GetInstance()->init(&my_io_service);

	MyZServerNetwork::GetInstance()->AcceptConnect();
	boost::thread t(boost::bind(&io_service::run, &my_io_service));

	//boost::thread dbThread(boost::bind(&DBSqlThread::run, DBSqlThread::GetInstance()));

	while (true)
	{
		MsgLogic::GetInstance()->GetvecMsg(MyZServerNetwork::GetInstance()->GetAllMsg());
		MsgLogic::GetInstance()->DealMsg();
		//MsgLogic::GetInstance()->DealSqlMsg();
	}

	system("pause");
	return 0;
}
