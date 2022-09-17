#pragma once
#include <vector>
#include "Common/GameHMsg.h"
#pragma comment(lib,"ProNetWork.lib")

namespace ProNetWork
{
	bool Connect(std::string IP, int Port);

	void DisConnect();

	void SendMsgToServer(HMessage & HMsg);

	std::vector< HMessage > RecvMsgFromServer();
}