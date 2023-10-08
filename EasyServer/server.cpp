#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<WinSock2.h>
#include<Windows.h>
#include<iostream>

#pragma comment (lib,"ws2_32.lib")

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR
};

struct DataHeader 
{
	short datalength;
	short cmd;
};

struct Login : public DataHeader
{
	Login()
	{
		this->cmd = CMD_LOGIN;
		this->datalength = sizeof(Login);
	}
	char userName[32];
	char PassWord[32];
};

struct LoginResult : public DataHeader
{
	LoginResult()
	{
		this->cmd = CMD_LOGIN_RESULT;
		this->datalength = sizeof(LoginResult);
		this->result = 0;
	}
	int result;
};

struct Logout : public DataHeader
{
	Logout()
	{
		this->cmd = CMD_LOGOUT;
		this->datalength = sizeof(Logout);
	}
	char userName[32];
};

struct LogoutResult : public DataHeader
{
	LogoutResult()
	{
		this->cmd = CMD_LOGOUT_RESULT;
		this->datalength = sizeof(LogoutResult);
		this->result = 0;
	}
	int result;
};


int main()
{
	//����Socket
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
	//����һ��Socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//�󶨽������ӿͻ��˵Ķ˿� bind 
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567); //host to net unsigned short
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;// Ĭ��IP��ַ�����ж����Է���//inet_addr("10.180.44.214");
	if (bind(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR)  //�󶨿���ʧ��,�ж�
	{
		std::cout << "����˿ڰ�ʧ�ܣ�" << std::endl;
	}
	else
	{
		std::cout << "����˿ڰ󶨳ɹ���" << std::endl;
	}
	//��������˿� listen
	if (listen(_sock, 5) == SOCKET_ERROR)
	{
		std::cout << "����˿ڼ���ʧ�ܣ�" << std::endl;
	}
	else
	{
		std::cout << "����˿ڼ����ɹ���" << std::endl;
	}
	//�ȴ����ܿͻ������� accept
	sockaddr_in ClientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
	_cSock = accept(_sock, (sockaddr*)&ClientAddr, &nAddrLen);
	if (_cSock == INVALID_SOCKET)
	{
		std::cout << "���ܵ���Ч�Ŀͻ�������!" << std::endl;
	}
	std::cout << "���յ��µĿͻ���IP:" << inet_ntoa(ClientAddr.sin_addr) << "\t"
		<< "socket��" << (int)_cSock << std::endl;
	while (true)
	{
		//������
		char SZrecv[4096] = {};
		int nlen = recv(_cSock, SZrecv, sizeof(DataHeader), 0);
		DataHeader* header = (DataHeader*)SZrecv;
		if (nlen <= 0)
		{
			std::cout << "�ͻ������˳���" << std::endl;
			break;
		}
		//��������
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			recv(_cSock, SZrecv + sizeof(DataHeader), header->datalength - sizeof(DataHeader), 0);
			Login* login = (Login*)SZrecv;
			std::cout << "���յ���Ϣ��username = " << login->userName << "\t" << "userpassword = " << login->PassWord << "\t"
				<< "datalength = " << login->datalength << std::endl;
			//�����ж��û������������Ĺ���
			LoginResult ret;
			send(_cSock, (const char*)&ret, sizeof(LoginResult), 0);
		}
		break;
		case CMD_LOGOUT:
		{
			recv(_cSock, SZrecv + sizeof(DataHeader), header->datalength - sizeof(DataHeader), 0);
			Logout* logout = (Logout*)SZrecv;
			std::cout << "���յ���Ϣ��username = " << logout->userName << "\t"
				<< "datalength = " << logout->datalength << std::endl;
			//�����ж��û������������Ĺ���
			LogoutResult ret;
			send(_cSock, (const char*)&ret, sizeof(LogoutResult), 0);
		}
		break;
		default:
			DataHeader header = { 0,CMD_ERROR };
			send(_cSock, (const char*)&header, sizeof(DataHeader), 0);
			break;
		}
	}
	//�ر�Socket
	closesocket(_sock);
	WSACleanup();
	system("pause");
	return 0;
}