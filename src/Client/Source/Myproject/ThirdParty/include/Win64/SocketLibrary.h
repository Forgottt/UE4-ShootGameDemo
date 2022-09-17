
#pragma once

namespace YJ
{
	//创建并且连接到服务器(in_IP地址,in_端口号,out_Socket)
	int CreateAndConnect(char IPAddress[256], int Htons, void*& Socket);

	//设置Socket为非阻塞模式;返回0:成功
	int Ioctlsocket(void* socket);

	//接收消息(in_Socket,in_数据,in_长度,in_标志位(默认0));返回实际接收到的消息长度
	int ReceiveMSG(void* Socket, char* Data, int Num, int Flags = 0);

	//发送消息(in_Socket,in_数据,in_长度,in_标志位(默认0));返回实际发出去的消息长度
	int SendMSG(void* Socket, char* Data, int Num, int Flags = 0);

	//关闭Socket
	int CloseSocket(void* Socket);
}