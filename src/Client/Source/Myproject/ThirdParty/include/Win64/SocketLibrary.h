
#pragma once

namespace YJ
{
	//�����������ӵ�������(in_IP��ַ,in_�˿ں�,out_Socket)
	int CreateAndConnect(char IPAddress[256], int Htons, void*& Socket);

	//����SocketΪ������ģʽ;����0:�ɹ�
	int Ioctlsocket(void* socket);

	//������Ϣ(in_Socket,in_����,in_����,in_��־λ(Ĭ��0));����ʵ�ʽ��յ�����Ϣ����
	int ReceiveMSG(void* Socket, char* Data, int Num, int Flags = 0);

	//������Ϣ(in_Socket,in_����,in_����,in_��־λ(Ĭ��0));����ʵ�ʷ���ȥ����Ϣ����
	int SendMSG(void* Socket, char* Data, int Num, int Flags = 0);

	//�ر�Socket
	int CloseSocket(void* Socket);
}