#pragma once
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <stdint.h>

enum {
	ERROR_CODE_NONE,
	ERROR_CODE_REGSTER_OK = 1,
	ERROR_CODE_REGSTER_FAIL = 2,
	ERROR_CODE_LOGIN_OK = 3,
	ERROR_CODE_LOGIN_FAIL = 4,
	ERROR_CODE_CHECK_DYNMIC_CODE_OK = 5,
	ERROR_CODE_CHECK_DYNMIC_CODE_FAIL = 6,
	ERROR_CODE_GET_ROLE_OK = 7,
	ERROR_CODE_GET_ROLE_FAIL = 8,
};
extern std::string g_strErrorCode[10000];
extern int g_iShutDown;

enum MainProtocol
{
	SP_TOOL_HEART_BEAT_MESSAGE = 111,
	SP_HEART_BEAT_MESSAGE = 0x111,
	SP_A_HEART_BEAT_MESSAGE = 0x112,
	SP_CHAT_HEART_BEAT_MESSAGE = 0x221,
	FM_ZDQ_SERVER_MESSAGE = 0x09,//9 ���������
	D_SERVER_LOGIN = 0x1001,


	FM_Q_ROUND_MESSAGE = 0x1037,//4151�����������Ϣ
	FM_Q_MAIN_MENU_MESSAGE = 0x1038,//4152�����ӵ���Э��
	FM_Q_HOME_MESSAGE = 0x1039,//4153,//�����԰������Ϣ
	FM_Q_ITEM_MESSAGE = 0x1040,//4160���ߵ�����Ϣ
	FM_Q_FIGHT_MESSAGE = 0x1041,//4161 fight������Ϣ
	FM_Q_SHOP_MESSAGE = 0x1042,//4162�̳ǵ���Ҫ����Ϣ
	FM_Q_GM_MESSAGE = 0x1043,//4163 GM����Ϣ
	FM_Q_SYSTEM_NOTICE_MESSAGE = 0x1044,//4164ϵͳ����ʾ����Ϣ�����԰�������Ϣ�ķ��ظ��ͻ��˵Ĵ�����ʾ��
	FM_Q_ROLE_MESSAGE = 0x1045,//4165��ɫ��ϵͳ����Ҫ����Ϣ�����ѵ�ϵͳ����ɫ��ת��ְҵ
	FM_Q_MISSION_MESSAGE = 0x1046,//4166����Э���
	FM_Q_NEWS_MESSAGE = 0x1047,//4167ϵͳ�����ţ������ʼ�������Э��
	FM_Q_ARENA_MESSAGE = 0x1048,//4168���������
	FM_Q_ACTIVE_MESSAGE = 0x1049,//4169 active���
	FM_Q_STATISTICS_MESSAGE = 0x1050,//4176 statistics���
	FM_Q_GROUP_MESSAGE = 0x1051,//4177 �������
	FM_Q_RANK_MESSAGE = 0x1052,//4178
	FM_ZDQ_LOGIN = 0x2001,//8193
	FM_BQ_DB_DATA_SAVE_AND_UPDATE = 0x2002,//8194
	FM_BLQ_PET_MESSAGE = 0x2003,//8195
	ZB_CHAT_MESSAGE = 0x2005,//8197

	ZB_ERROR_MESSAGE = 0x2006,

};

enum SubProtocol
{
	C2ZS_R_REGISTER = 1,//����ע�� request
	ZS2C_A_REGISTER = 2,//�ظ�ע�� Answer
	C2ZS_R_LOGIN = 3,
	ZS2C_A_LOGIN = 4,//�ظ��ͻ��˵�½���󲢻�ô���
	C2DS_R_ALLROLE = 5,//�ͻ��������ý�ɫ
	DS2C_A_ALLROLE = 6,//�ظ��ͻ��������ý�ɫ
	DS2ZS_R_CHECKDYNMICCODE = 1000,
	ZS2DS_A_CHECKDYNMICCODE = 1001,
	C2GS_R_CHECKDYNMICCODE = 1002,
	GS2C_A_CHECKDYNMICCODE = 1003,
	GS2DS_R_CHECKDYNMICCODE = 1004,
	DS2GS_A_CHECKDYNMICCODE = 1005,
};


//���л� string HMessage potocol
class HMessage
{
#ifndef RELASE_
	unsigned char m_byBuffer[10000];
#endif
	unsigned char *m_pData;
	int m_iPos;//ʵ��ѹ�����ݵ�λ�ã����ߵ�ǰ��ȡ��λ��
	int m_iDataUseSize;//ʵ��ѹ�����ݵĴ�С

#ifdef RELASE_
	int m_iDataMaxSize;//10000
#endif
public:
	int m_iSessionID;
	int m_DynmicCode;
#ifdef RELASE_
	void Resize(int iSize)
	{
		pData = new unsigned char[iSize];
		memcpy(pData, m_pData, m_iDataMaxSize);
		m_pData = pData;
		m_iDataMaxSize = iSize;
	}
#endif
	HMessage()
	{
#ifdef RELASE_
		m_iDataMaxSize = 10000;
		m_pData = new unsigned char[m_iDataMaxSize];
#else
		m_pData = m_byBuffer;
#endif
		m_iPos = 0;
		m_iDataUseSize = 0;
	}

	HMessage(const char * data, int iLen)
	{
		m_iPos = 0;
		m_iDataUseSize = iLen;
		m_pData = m_byBuffer;
		memcpy(m_pData, data, iLen);

	}
	unsigned char * GetData()
	{
		return m_byBuffer;
	}
	int GetPosition()
	{

		*((uint16_t*)(m_pData + 0)) = (uint16_t)m_iPos;//���¸��³���
		return m_iPos;
	}
	void AddPosition(int iAddition)
	{
		m_iPos += iAddition;
		if (m_iPos > m_iDataUseSize)
			m_iDataUseSize = m_iPos;
	}

	bool putChar(uint8_t  data)
	{
		if (CheckPut(sizeof(data)) == false)
		{
			return false;
		}
		*((uint8_t*)(m_pData + m_iPos)) = data;

		AddPosition(sizeof(data));
		return true;
	}
	void SetPos(int iPos)
	{
		this->m_iPos = iPos;
	}
	bool putUInt(uint32_t data)
	{
		if (CheckPut(sizeof(data)) == false)
		{
			return false;
		}

		uint8_t temp = data >> 24;
		*(m_pData + m_iPos + 0) = temp;

		temp = (data >> 16);
		*(m_pData + m_iPos + 1) = temp & 0xff;//0x00001234

		temp = (data >> 8);
		*(m_pData + m_iPos + 2) = temp & 0xff;//0x00123456

		temp = (data);
		*(m_pData + m_iPos + 3) = temp & 0xff;//0x12345678

		AddPosition(sizeof(data));
		return true;
	}
	bool putUShort(uint16_t data)
	{
		if (CheckPut(sizeof(data)) == false)
		{
			return false;
		}
		*((uint16_t*)(m_pData + m_iPos)) = data;

		AddPosition(sizeof(data));
		return true;
	}

	void getUShort(uint16_t & data)
	{

		data = *((uint16_t*)(m_pData + m_iPos));

		AddPosition(sizeof(data));
	}
	void getUInt(uint32_t & data)
	{
		data = 0;
		data |= *(m_pData + m_iPos + 0) << 24;
		data |= *(m_pData + m_iPos + 1) << 16;
		data |= *(m_pData + m_iPos + 2) << 8;
		data |= *(m_pData + m_iPos + 3);

		AddPosition(sizeof(data));
	}

	void getByte(uint8_t & data)
	{
		data = 0;
		data = m_pData[m_iPos];
		AddPosition(sizeof(data));
	}

	bool CheckPut(int iSize)
	{

		if (m_iPos + iSize < 10000)
		{
			return true;
		}

#ifdef RELASE_
		bool bCheck = Resize(10000 + iSize);
		if (bCheck == true)return true;
#endif
		return false;

	}

	bool putCharArray(const char *pData, int iLen)
	{
		if (CheckPut(iLen) == false)
		{
			return false;
		}
		memcpy(m_pData + m_iPos, pData, iLen);
		m_iPos += iLen;
		return true;
	}

	bool putString(const std::string & str)
	{
		int iLen = str.length();
		if (false == putUShort(iLen))
			return false;

		return putCharArray(str.c_str(), iLen);

	}
	void getString(std::string & str)
	{
		uint16_t iLen = 0;
		getUShort(iLen);
		char *pData = new char[iLen + 1];
		memcpy(pData, m_pData + m_iPos, iLen);
		m_iPos += iLen;
		pData[iLen] = '\0';
		str = pData;
		delete[] pData;
	}
};

//һ�����Ϣ�ṹ
class BaseMessage
{
public:
	uint16_t m_wSize;//���ݳ���
	uint16_t m_iMainProtocol;//��Э����
	uint16_t m_iSubProtocol;//��Э����
	virtual void put(HMessage & hmg)
	{
		hmg.putUShort(m_wSize);
		hmg.putUShort(m_iMainProtocol);
		hmg.putUShort(m_iSubProtocol);
	}
	virtual void get(HMessage & hmg)
	{
		hmg.getUShort(m_wSize);
		hmg.getUShort(m_iMainProtocol);
		hmg.getUShort(m_iSubProtocol);
	}

	virtual BaseMessage & operator =(const BaseMessage & tmp)//����=
	{
		m_wSize = tmp.m_wSize;
		m_iMainProtocol = tmp.m_iMainProtocol;
		m_iSubProtocol = tmp.m_iSubProtocol;
		return *this;
	}

	void clear()
	{
		m_iSubProtocol = 0;
		m_iMainProtocol = 0;
		m_wSize = 0;
	}
};

//�ͻ��������¼�ʺŷ�����
class C2ZS_R_Login :public BaseMessage
{
public:
	std::string m_strAccount;
	std::string m_strPassword;
	C2ZS_R_Login()
	{
		m_iMainProtocol = FM_ZDQ_LOGIN;
		m_iSubProtocol = C2ZS_R_LOGIN;
	}

	virtual void put(HMessage & hmg)
	{
		BaseMessage::put(hmg);
		hmg.putString(m_strAccount);
		hmg.putString(m_strPassword);
	}
	virtual void get(HMessage & hmg)
	{
		// BaseMessage::get(hmg);
		hmg.getString(m_strAccount);
		hmg.getString(m_strPassword);
	}
};

//����˻ظ���¼�ʺŷ�����
class ZS2C_A_Login :public BaseMessage
{
public:
	uint32_t m_iErrorCode;
	uint32_t m_DynmicCode;
public:
	uint32_t m_iCount;//��������������
	uint32_t m_iID;//ID
	std::string m_strName;//����
	std::string m_strIP;
	uint16_t m_wPort;
	uint8_t m_State;

	ZS2C_A_Login()
	{
		m_iMainProtocol = FM_ZDQ_LOGIN;
		m_iSubProtocol = ZS2C_A_LOGIN;
		m_DynmicCode = 0;
		m_iCount = 0;
		m_iID = 0;
		m_strName = "";
		m_strIP = "";
		m_wPort = 0;
		m_State = -1;
	}
	virtual void put(HMessage & hmg)
	{
		BaseMessage::put(hmg);
		hmg.putUInt(m_iErrorCode);
		hmg.putUInt(m_DynmicCode);
		hmg.putUInt(m_iCount);
	}

	void putServerData(HMessage &hmg)
	{
		hmg.putUInt(m_iID);
		hmg.putString(m_strName);
		hmg.putString(m_strIP);
		hmg.putUShort(m_wPort);
		hmg.putChar(m_State);
	}

	virtual void get(HMessage & hmg)
	{
		hmg.getUInt(m_iErrorCode);
		hmg.getUInt(m_DynmicCode);
		hmg.getUInt(m_iCount);
	}

	void getServerData(HMessage & hmg)
	{
		hmg.getUInt(m_iID);
		hmg.getString(m_strName);
		hmg.getString(m_strIP);
		hmg.getUShort(m_wPort);
		hmg.getByte(m_State);
	}
};

//�ͻ����������ʺŷ�����ע���˺�
class C2ZS_R_RegisterAccount :public BaseMessage
{
public:
	std::string m_strAccount;
	std::string m_strPassword;
	C2ZS_R_RegisterAccount()
	{
		m_iMainProtocol = FM_ZDQ_LOGIN;
		m_iSubProtocol = C2ZS_R_REGISTER;
	}
	virtual void put(HMessage & hmg)
	{
		BaseMessage::put(hmg);
		hmg.putString(m_strAccount);
		hmg.putString(m_strPassword);
	}
	virtual void get(HMessage & hmg)
	{
		//BaseMessage::get(hmg);
		hmg.getString(m_strAccount);
		hmg.getString(m_strPassword);
	}
};

//����˻ظ��ͻ������ʺŷ����������ע��
class ZS2C_A_RegisterAccount :public BaseMessage
{
public:
	uint32_t m_iErrorCode;
	uint32_t m_iErrorCodeIndex;//��������  -4  �ظ�ע��
	ZS2C_A_RegisterAccount()
	{
		m_iMainProtocol = FM_ZDQ_LOGIN;
		m_iSubProtocol = ZS2C_A_REGISTER;
	}
	virtual void put(HMessage & hmg)
	{
		BaseMessage::put(hmg);
		hmg.putUInt(m_iErrorCode);
		hmg.putUInt(m_iErrorCodeIndex);
	}
	virtual void get(HMessage & hmg)
	{
		hmg.getUInt(m_iErrorCode);
	}
};

//�ͻ��������ý�ɫ��Ϣ
class C2DS_R_GetAllRole :public BaseMessage
{
public:
	uint32_t m_DynmicCode;//�ͻ��˶�̬��
	uint32_t m_SessionID;//�ͻ���ID
	C2DS_R_GetAllRole()
	{
		m_iMainProtocol = FM_ZDQ_LOGIN;
		m_iSubProtocol = C2DS_R_ALLROLE;
	}
	virtual void put(HMessage & hmg)
	{
		BaseMessage::put(hmg);
		hmg.putUInt(m_DynmicCode);
		hmg.putUInt(m_SessionID);
	}
	virtual void get(HMessage & hmg)
	{
		hmg.getUInt(m_DynmicCode);
		hmg.getUInt(m_SessionID);
	}
};

//�ظ������ý�ɫ��Ϣ
class DS2C_A_GetAllRole :public BaseMessage
{
public:
	uint32_t m_iCount;//��ɫ����
	std::string m_rolename;
	DS2C_A_GetAllRole()
	{
		m_iMainProtocol = FM_ZDQ_LOGIN;
		m_iSubProtocol = DS2C_A_ALLROLE;
	}
	virtual void put(HMessage & hmg)
	{
		BaseMessage::put(hmg);
		hmg.putUInt(m_iCount);
	}
	void putRole(HMessage & hmg)
	{
		hmg.putString(m_rolename);
	}
	virtual void get(HMessage & hmg)
	{
		hmg.getString(m_rolename);
	}
	void getRole(HMessage & hmg)
	{
		hmg.getString(m_rolename);
	}
};


//�������������ʺŷ�������֤��̬��
class DS2ZS_R_CheckDynmicCode :public BaseMessage
{
public:
	uint32_t m_DynmicCode;//ת���ͻ��˶�̬��
	uint32_t m_SessionID;//ת���ͻ���ID
	uint32_t m_SessionIDFromDS;//�����Ŀͻ���ID
	DS2ZS_R_CheckDynmicCode()
	{
		m_iMainProtocol = FM_Q_ROUND_MESSAGE;
		m_iSubProtocol = DS2ZS_R_CHECKDYNMICCODE;
	}
	virtual void put(HMessage & hmg)
	{
		BaseMessage::put(hmg);
		hmg.putUInt(m_DynmicCode);
		hmg.putUInt(m_SessionID);
		hmg.putUInt(m_SessionIDFromDS);
	}
	virtual void get(HMessage & hmg)
	{
		hmg.getUInt(m_DynmicCode);
		hmg.getUInt(m_SessionID);
		hmg.getUInt(m_SessionIDFromDS);
	}
};

//�ʺŷ�����������������ظ���̬����֤
class ZS2DS_A_CheckDynmicCode :public BaseMessage
{
public:
	uint32_t m_isOK;//ERROR_CODE_REGSTER_OK  OK  ERROR_CODE_REGSTER_FAIL  NOT OK
	uint32_t m_SessionIDFromDS;//�����Ŀͻ���ID
	ZS2DS_A_CheckDynmicCode()
	{
		m_iMainProtocol = FM_Q_ROUND_MESSAGE;
		m_iSubProtocol = ZS2DS_A_CHECKDYNMICCODE;
	}
	virtual void put(HMessage & hmg)
	{
		BaseMessage::put(hmg);
		hmg.putUInt(m_isOK);
		hmg.putUInt(m_SessionIDFromDS);
	}
	virtual void get(HMessage & hmg)
	{
		hmg.getUInt(m_isOK);
		hmg.getUInt(m_SessionIDFromDS);
	}
};

//��Ϸ�������ظ�������Ϸ
class GS2C_A_Login :public BaseMessage
{
public:
	uint32_t m_iErrorCode;
	GS2C_A_Login()
	{
		m_iMainProtocol = FM_ZDQ_LOGIN;
		m_iSubProtocol = GS2C_A_CHECKDYNMICCODE;
	}
	virtual void put(HMessage & hmg)
	{
		BaseMessage::put(hmg);
		hmg.putUInt(m_iErrorCode);
	}
	virtual void get(HMessage & hmg)
	{
		hmg.getUInt(m_iErrorCode);
	}
};

//��Ϸ�������������������֤��̬��
class GS2DS_R_CheckDynmicCode :public BaseMessage
{
public:
	uint32_t m_DynmicCode;//ת���ͻ��˶�̬��
	uint32_t m_SessionID;//ת���ͻ���ID
	GS2DS_R_CheckDynmicCode()
	{
		m_iMainProtocol = FM_Q_ROUND_MESSAGE;
		m_iSubProtocol = GS2DS_R_CHECKDYNMICCODE;
	}
	virtual void put(HMessage & hmg)
	{
		BaseMessage::put(hmg);
		hmg.putUInt(m_DynmicCode);
		hmg.putUInt(m_SessionID);
	}
	virtual void get(HMessage & hmg)
	{
		hmg.getUInt(m_DynmicCode);
		hmg.getUInt(m_SessionID);
	}
};

//��������������Ϸ�������ظ���̬����֤
class DS2GS_A_CheckDynmicCode :public BaseMessage
{
public:
	uint32_t m_isOK;//ERROR_CODE_REGSTER_OK  OK  ERROR_CODE_REGSTER_FAIL  NOT OK
	uint32_t m_SessionID;//��Ϸ����������ͻ���ID
	DS2GS_A_CheckDynmicCode()
	{
		m_iMainProtocol = FM_Q_ROUND_MESSAGE;
		m_iSubProtocol = DS2GS_A_CHECKDYNMICCODE;
	}
	virtual void put(HMessage & hmg)
	{
		BaseMessage::put(hmg);
		hmg.putUInt(m_isOK);
		hmg.putUInt(m_SessionID);
	}
	virtual void get(HMessage & hmg)
	{
		hmg.getUInt(m_isOK);
		hmg.getUInt(m_SessionID);
	}
};

//�ͻ������������Ϸ
class C2GS_R_Login :public BaseMessage
{
public:
	C2GS_R_Login()
	{
		m_iMainProtocol = FM_ZDQ_LOGIN;
		m_iSubProtocol = C2GS_R_CHECKDYNMICCODE;
	}
	virtual void put(HMessage & hmg)
	{
		BaseMessage::put(hmg);
	}
	virtual void get(HMessage & hmg)
	{
	}
};

//������
class ZS2C_A_ErrorMessage :public BaseMessage
{
public:
	ZS2C_A_ErrorMessage()
	{
		m_iMainProtocol = ZB_ERROR_MESSAGE;
		m_iSubProtocol = 2;
	}
	uint32_t m_iErrorCode;
	virtual void put(HMessage & hmg)
	{
		BaseMessage::put(hmg);
		hmg.putUInt(m_iErrorCode);
	}
	virtual void get(HMessage & hmg)
	{
		hmg.getUInt(m_iErrorCode);
	}
};