#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <time.h>
#include <stdint.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
using namespace boost::asio;

typedef boost::shared_lock<boost::shared_mutex> readLock;
typedef boost::unique_lock<boost::shared_mutex> writeLock;

//ǰ��������Ϣ��
class HMessage;

//�ͻ�����
class SessionClient
{
public:
	int m_iID;
	int m_iQQ;
	uint32_t m_DynmicCode;
	char m_szSendBuffer[4000];
	boost::shared_ptr<ip::tcp::socket> m_pSocketClient;
};

//ZS������
class MyZServerNetwork
{
	std::map<int, int > m_mapAllPlayer;
	std::map<int, boost::shared_ptr<SessionClient> > m_mapAllClientSock;
	boost::shared_mutex m_mapAllClientSockLock;
	std::vector< boost::shared_ptr<HMessage> > m_vecAllClientMsg;
	boost::shared_mutex m_vecAllClientMsgLock;

	static int g_iCurrentID;
	boost::shared_ptr<ip::tcp::acceptor> m_pAcceptor;

	io_service * m_pIoService;
public:
	void SendMsgByPlayerID(int iPlayerID, HMessage* hmg);
	void SendMsgBySessionID(int iSessionID, HMessage* hmg);
	void SendMsgToAll();
public:
	void init(io_service * pIoService);
	static MyZServerNetwork * GetInstance();

	void Handle_accept(boost::shared_ptr<SessionClient> pCurrentSession, const boost::system::error_code & error);
	void Handle_Read(boost::shared_ptr<SessionClient> pSessionClient, const boost::system::error_code & error, const size_t &iLen);

	//�ӿں������νӺ���
	std::vector< boost::shared_ptr<HMessage> > GetAllMsg();
	std::map<int, boost::shared_ptr<SessionClient>> GetAllClient();

	//���߳�
	void AcceptConnect();//�������ӣ���������

};
