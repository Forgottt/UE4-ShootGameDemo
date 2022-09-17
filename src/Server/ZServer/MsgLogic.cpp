#include "MyZServerNetwork.h"
#include "MyMsg.h"
#include "MsgLogic.h"

struct ServerData
{
	uint32_t dwID;
	std::string strName;
	std::string strIP;
	uint16_t    wPort;
	uint8_t     byState;
};
std::vector<ServerData > g_vecServerList;

MsgLogic* MsgLogic::GetInstance()
{
	static MsgLogic* pInstance = NULL;
	if (pInstance == NULL)
	{
		pInstance = new MsgLogic();
	}
	return pInstance;
}

void MsgLogic::GetvecMsg(std::vector<boost::shared_ptr<HMessage>> vecmsg)
{
	m_vecAllClientMsg = vecmsg;
}

void MsgLogic::GetmapAllClient(std::map<int, boost::shared_ptr<SessionClient> > mapClient)
{
	m_mapAllClient = mapClient;
}

void MsgLogic::DealMsg()//处理消息
{
	if (m_vecAllClientMsg.empty())
		return;

	for (unsigned int i = 0; i < m_vecAllClientMsg.size(); i++)
	{
		boost::shared_ptr<HMessage> hmg;
		hmg = m_vecAllClientMsg[i];
		BaseMessage baseMsg;
		baseMsg.get(*hmg);
		if (baseMsg.m_iMainProtocol == FM_ZDQ_LOGIN)
		{
			//注册
			if (baseMsg.m_iSubProtocol == C2ZS_R_REGISTER)
			{
				C2ZS_R_RegisterAccount recvMsg;
				*((BaseMessage*)(&recvMsg)) = baseMsg;
				recvMsg.get(*hmg);

				//boost::shared_ptr< CWaitNode > CNode = boost::shared_ptr< CWaitNode >(new CRegisterWaitNode(recvMsg.m_strAccount, recvMsg.m_strPassword));
				//DBSqlThread::GetInstance()->AddWaitNode(CNode);
			}
			//登录
			else if (baseMsg.m_iSubProtocol == C2ZS_R_LOGIN)
			{
				C2ZS_R_Login recvMsg;
				*((BaseMessage*)(&recvMsg)) = baseMsg;
				recvMsg.get(*hmg);

				ZS2C_A_Login sendMsg;
				sendMsg.m_iErrorCode = ERROR_CODE_LOGIN_OK;
				sendMsg.m_DynmicCode = 0;
				HMessage sendHmg;
				sendMsg.put(sendHmg);
				MyZServerNetwork::GetInstance()->SendMsgBySessionID(hmg->m_iSessionID, &sendHmg);
				//boost::shared_ptr< CWaitNode > CNode = boost::shared_ptr< CWaitNode >(new CLoginWaitNode(recvMsg.m_strAccount, recvMsg.m_strPassword,hmg->m_DynmicCode));
				//DBSqlThread::GetInstance()->AddWaitNode(CNode);
			}
		}
		//动态码验证
		else if (baseMsg.m_iMainProtocol == FM_Q_ROUND_MESSAGE)
		{
			if (baseMsg.m_iSubProtocol == DS2ZS_R_CHECKDYNMICCODE)
			{
				DS2ZS_R_CheckDynmicCode recvMsg;
				*((BaseMessage*)(&recvMsg)) = baseMsg;
				recvMsg.get(*hmg);
				if (CheckDynmicCode(recvMsg.m_SessionID, recvMsg.m_DynmicCode))
				{
					//验证成功
					ZS2DS_A_CheckDynmicCode sendMsg;
					sendMsg.m_isOK = ERROR_CODE_CHECK_DYNMIC_CODE_OK;
					sendMsg.m_SessionIDFromDS = recvMsg.m_SessionIDFromDS;
					HMessage sendHmg;
					sendMsg.put(sendHmg);
					MyZServerNetwork::GetInstance()->SendMsgBySessionID(hmg->m_iSessionID, &sendHmg);
				}
				else
				{
					//验证失败
					ZS2DS_A_CheckDynmicCode sendMsg;
					sendMsg.m_isOK = ERROR_CODE_CHECK_DYNMIC_CODE_FAIL;
					sendMsg.m_SessionIDFromDS = recvMsg.m_SessionIDFromDS;
					HMessage sendHmg;
					sendMsg.put(sendHmg);
					MyZServerNetwork::GetInstance()->SendMsgBySessionID(hmg->m_iSessionID, &sendHmg);
				}
			}
		}
	}
	m_vecAllClientMsg.clear();
}

void MsgLogic::DealSqlMsg()
{
	m_vecAllFinishWaitNode = DBSqlThread::GetInstance()->GetFinishDBWait();

	if (m_vecAllFinishWaitNode.empty())
		return;

	for (unsigned int i = 0; i < m_vecAllFinishWaitNode.size(); i++)
	{
		m_vecAllFinishWaitNode[i]->DoLogic();
	}

	m_vecAllFinishWaitNode.clear();
}

//动态码验证
bool MsgLogic::CheckDynmicCode(int id ,int dc)
{
	std::map<int, boost::shared_ptr<SessionClient>>::iterator it;
	for (it = m_mapAllClient.begin(); it != m_mapAllClient.end(); ++it)
	{
		if (id == it->second->m_iID && dc == it->second->m_DynmicCode)
			return true;
	}
	return false;
}
