#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<WinSock2.h>
#include<Windows.h>
#include<iostream>
#pragma comment (lib,"ws2_32.lib")

enum CMD
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR
};

struct DataHeader
{
	short datalength;
	short cmd;
};

struct Login
{
	char userName[32];
	char PassWord[32];
};

struct LoginResult
{
	int result;
};

struct Logout
{
	char userName[32];
};

struct LogoutResult
{
	int result;
};


int main()
{
	//����Socket
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
	//����socket
	SOCKET _cSock = socket(AF_INET, SOCK_STREAM, 0);
	if (_cSock == INVALID_SOCKET)
	{
		std::cout << "�׽��ֽ���ʧ�ܣ�" << std::endl;
	}
	else
	{
		std::cout << "�׽��ֽ����ɹ�!" << std::endl;
	}
	//���ӷ����� connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("10.180.46.33");
	if (connect(_cSock, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		std::cout << "����������ʧ�ܣ�" << std::endl;
	}
	else
	{
		std::cout << "���������ӳɹ�!" << std::endl;
	}
	//��������
	while (true)
	{
		char msgBuf[256] = {};
		std::cout << "����������" << std::endl;
		std::cin >> msgBuf;
		//��������
		if (0 == strcmp(msgBuf, "exit"))
		{
			std::cout << "�ͻ����˳���" << std::endl;
			break;
		}
		if (0 == strcmp(msgBuf, "login"))
		{
			Login login = {"����","abcdef"};
			DataHeader header = {sizeof(Login),CMD_LOGIN};
			//��������
			send(_cSock, (const char*)&header, sizeof(DataHeader), 0);
			send(_cSock, (const char*)&login, sizeof(Login), 0);
			//������Ϣ
			LoginResult loginret = {};
			DataHeader headerret = {};
			recv(_cSock, (char*)&headerret, sizeof(DataHeader), 0);
			recv(_cSock, (char*)&loginret, sizeof(LoginResult), 0);
			std::cout << "LoginResult ��" << loginret.result << std::endl;
		}
		if (0 == strcmp(msgBuf, "logout"))
		{
			Logout logout = { "����" };
			DataHeader header = { sizeof(Logout),CMD_LOGOUT };
			//��������
			send(_cSock, (const char*)&header, sizeof(DataHeader), 0);
			send(_cSock, (const char*)&logout, sizeof(Logout), 0);
			//������Ϣ
			LogoutResult logoutret = {};
			DataHeader headerret = {};
			recv(_cSock, (char*)&headerret, sizeof(DataHeader), 0);
			recv(_cSock, (char*)&logoutret, sizeof(LogoutResult), 0);
			std::cout << "LogoutResult : " << logoutret.result << std::endl;
		}
		if(0 != strcmp(msgBuf, "logout") && 0 != strcmp(msgBuf, "login"))
		{
			std::cout << "��֧�ֵ�������������룡" << std::endl;
		}
	}
	//�ر��׽���
	closesocket(_cSock);
	//�ر�Socket
	WSACleanup();
	system("pause");
	return 0;
}