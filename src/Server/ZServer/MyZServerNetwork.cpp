#include "MyZServerNetwork.h"
#include "MyMsg.h"

void MyZServerNetwork::SendMsgByPlayerID(int iPlayerID, HMessage* hmg)
{
	int iSessionID = m_mapAllPlayer[iPlayerID];
	SendMsgBySessionID(iSessionID, hmg);
}

void MyZServerNetwork::SendMsgBySessionID(int iSessionID, HMessage* hmg)
{
	hmg->putChar('\n');
	m_mapAllClientSock[iSessionID]->m_pSocketClient->write_some(buffer(hmg->GetData(), hmg->GetPosition()));
}

void MyZServerNetwork::init(io_service * pIoService)
{
	ip::tcp::endpoint  local_point = ip::tcp::endpoint(ip::address_v4::from_string("0.0.0.0"), 2300);
	m_pAcceptor = boost::shared_ptr<ip::tcp::acceptor>(new ip::tcp::acceptor(*pIoService, local_point));
	m_pIoService = pIoService;
}

MyZServerNetwork * MyZServerNetwork::GetInstance()
{
	static MyZServerNetwork * pInstance = NULL;
	if (pInstance == NULL)
	{
		pInstance = new MyZServerNetwork();
	}
	return pInstance;
}

std::vector< boost::shared_ptr<HMessage> > MyZServerNetwork::GetAllMsg()
{
	std::vector< boost::shared_ptr<HMessage> > vecTmpMsgList;
	{
		writeLock writelock(m_vecAllClientMsgLock);
		vecTmpMsgList = m_vecAllClientMsg;//所有客户端的消息
		m_vecAllClientMsg.clear();
	}

	return vecTmpMsgList;
}


std::map<int, boost::shared_ptr<SessionClient>> MyZServerNetwork::GetAllClient()
{
	std::map<int, boost::shared_ptr<SessionClient>> mapTmpMsgList;
	{
		writeLock writelock(m_mapAllClientSockLock);
		mapTmpMsgList = m_mapAllClientSock;//所有客户端
	}
	return mapTmpMsgList;
}

void MyZServerNetwork::Handle_Read(boost::shared_ptr<SessionClient> pSessionClient, const boost::system::error_code & error, const size_t &iLen)
{
	if (0 != error)//0是正常的
	{
		//客户端优雅的断开
		{
			writeLock writeLockAllClientSock(m_mapAllClientSockLock);
			std::map<int, boost::shared_ptr<SessionClient> >::iterator it = m_mapAllClientSock.find(pSessionClient->m_iID);
			if (it != m_mapAllClientSock.end())
				m_mapAllClientSock.erase(it);
		}
	}

	try
	{
		boost::shared_ptr<HMessage> pHmg = boost::shared_ptr<HMessage>(new HMessage(pSessionClient->m_szSendBuffer, iLen));
		pHmg->m_iSessionID = pSessionClient->m_iID;
		pHmg->m_DynmicCode = pSessionClient->m_DynmicCode;
		{
			writeLock writelock(m_vecAllClientMsgLock);
			m_vecAllClientMsg.push_back(pHmg);
		}
		pSessionClient->m_pSocketClient->async_read_some(buffer(pSessionClient->m_szSendBuffer, 4000), boost::bind(&MyZServerNetwork::Handle_Read, this, pSessionClient, _1, _2));
	}
	catch (std::exception & e)//断线等异常处理
	{
		std::cout << e.what() << " \n";
	}
}

void MyZServerNetwork::Handle_accept(boost::shared_ptr<SessionClient> pCurrentSession, const boost::system::error_code & error)
{
	try
	{
		pCurrentSession->m_pSocketClient->async_read_some(buffer(pCurrentSession->m_szSendBuffer, 4000), boost::bind(&MyZServerNetwork::Handle_Read, this, pCurrentSession, _1, _2));
	}
	catch (std::exception & e)//异常处理
	{
		std::cout << e.what() << " \n";
	}
	{
		writeLock writeLockAllClientSock(m_mapAllClientSockLock);
		m_mapAllClientSock[g_iCurrentID] = pCurrentSession;
	}


	boost::shared_ptr<ip::tcp::socket> pSock = boost::shared_ptr<ip::tcp::socket>(new ip::tcp::socket(*m_pIoService));
	g_iCurrentID++;
	if (g_iCurrentID > 999999999)
	{
		g_iCurrentID = 1;
	}
	boost::shared_ptr<SessionClient> pNextSession = boost::shared_ptr<SessionClient>(new SessionClient);
	pNextSession->m_iID = g_iCurrentID;
	pNextSession->m_pSocketClient = pSock;
	this->m_pAcceptor->async_accept(*pSock, boost::bind(&MyZServerNetwork::Handle_accept, this, pNextSession, _1));
}

void MyZServerNetwork::AcceptConnect()
{
	boost::shared_ptr<ip::tcp::socket> pSock = boost::shared_ptr<ip::tcp::socket>(new ip::tcp::socket(*m_pIoService));
	g_iCurrentID++;
	if (g_iCurrentID > 999999999)
	{
		g_iCurrentID = 1;
	}

	boost::shared_ptr<SessionClient> pSession = boost::shared_ptr<SessionClient>(new SessionClient);

	pSession->m_iID = g_iCurrentID;
	pSession->m_DynmicCode = rand();
	pSession->m_pSocketClient = pSock;

	this->m_pAcceptor->async_accept(*pSock, boost::bind(&MyZServerNetwork::Handle_accept, this, pSession, _1));
}
