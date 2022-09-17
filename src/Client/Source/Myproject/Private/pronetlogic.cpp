// Fill out your copyright notice in the Description page of Project Settings.

#include "ProNetLogic.h"

UProNetLogic * UProNetLogic::m_pNetLogic = nullptr;
UProNetLogic::~UProNetLogic()
{
	if (nullptr != m_pNetLogic)
		m_pNetLogic = nullptr;
}

UProNetLogic * UProNetLogic::GetInstance()
{
	if (nullptr == m_pNetLogic)
	{
		m_pNetLogic = NewObject< UProNetLogic >();
		m_pNetLogic->m_isConneted = false;
	}
	return m_pNetLogic;
}

void UProNetLogic::Connect(FString IP, int Port)
{
	if (m_isConneted)
	{
		//断开连接接口
		ProNetWork::DisConnect();
	}

	FString temp = FPaths::GameDir();
	//读取配置文件IP和端口
	FString IP_ini;
	int Port_ini;
	GConfig->GetString(
		TEXT("IP/Port"),
		TEXT("IPAddress"),
		IP_ini,
		FPaths::GameDir() / "IpConfig.ini"
	);

	GConfig->GetInt(
		TEXT("IP/Port"),
		TEXT("Port"),
		Port_ini,
		FPaths::GameDir() / "IpConfig.ini"
	);

	m_isConneted = ProNetWork::Connect(TCHAR_TO_UTF8(*IP_ini), Port_ini);
}

void UProNetLogic::GameLogicRegister(FString account, FString password)
{
	HMessage HMsg;
	C2ZS_R_RegisterAccount SendMsg;
	SendMsg.m_strAccount = TCHAR_TO_UTF8(*account);
	SendMsg.m_strPassword = TCHAR_TO_UTF8(*password);
	SendMsg.put(HMsg);

	ProNetWork::SendMsgToServer(HMsg);
}


void UProNetLogic::GameLogicLogin(FString account, FString password)
{
	HMessage HMsg;
	C2ZS_R_Login SendMsg;
	SendMsg.m_strAccount = TCHAR_TO_UTF8(*account);
	SendMsg.m_strPassword = TCHAR_TO_UTF8(*password);
	SendMsg.put(HMsg);

	ProNetWork::SendMsgToServer(HMsg);
}

void UProNetLogic::OnLogin(FOnLoginDelegate onlogin)
{
	m_OnLoginDelegate = onlogin;
}

void UProNetLogic::OnRegister(FOnRegistDelegate onregist)
{
	m_OnRegistDelegate = onregist;
}

void UProNetLogic::DealMsg()
{
	std::vector< HMessage > m_vecRecvMsg = ProNetWork::RecvMsgFromServer();
	for (size_t i = 0; i < m_vecRecvMsg.size(); ++i)
	{
		HMessage hmg;
		hmg = m_vecRecvMsg[i];
		BaseMessage baseMsg;
		baseMsg.get(hmg);
		switch (baseMsg.m_iMainProtocol)
		{
		case FM_ZDQ_LOGIN:
		{
			switch (baseMsg.m_iSubProtocol)
			{
			case ZS2C_A_REGISTER:
			{
				ZS2C_A_RegisterAccount recvMsg;
				recvMsg.get(hmg);
				if (ERROR_CODE_REGSTER_OK == recvMsg.m_iErrorCode)
					m_OnRegistDelegate.Execute(true);
				else
					m_OnRegistDelegate.Execute(false);
				break;
			}
			case ZS2C_A_LOGIN:
			{
				ZS2C_A_Login recvMsg;
				recvMsg.get(hmg);
				if (ERROR_CODE_LOGIN_OK == recvMsg.m_iErrorCode)
					m_OnLoginDelegate.Execute(true);
				else
					m_OnLoginDelegate.Execute(false);
				break;
			}
			default:
			{
				UE_LOG(GameLog, Error, TEXT("%s"), "Unknow MSG(Sub)");
				break;
			}
			}
		}
		default:
		{
			UE_LOG(GameLog, Error, TEXT("%s"), "Unknow MSG(Main)");
			break;
		}
		}
	}
}