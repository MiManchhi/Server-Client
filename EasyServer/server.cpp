#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<WinSock2.h>
#include<Windows.h>
#include<iostream>
#include<vector>

#pragma comment (lib,"ws2_32.lib")

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_ERROR,
	CMD_NEW_CLIENT_JOIN
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

struct NewClientJoin : public DataHeader
{
	NewClientJoin()
	{
		this->cmd = CMD_NEW_CLIENT_JOIN;
		this->datalength = sizeof(NewClientJoin);
		this->sock = 0;
	}
	int sock;
};

std::vector<SOCKET>g_clients;

int processor(SOCKET _cSock)
{
	//������
	char SZrecv[4096] = {};
	int nlen = recv(_cSock, SZrecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)SZrecv;
	if (nlen <= 0)
	{
		std::cout << "�ͻ���" << (int)_cSock << "���˳���" << std::endl;
		return -1;
	}
	//��������
	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		recv(_cSock, SZrecv + sizeof(DataHeader), header->datalength - sizeof(DataHeader), 0);
		Login* login = (Login*)SZrecv;
		std::cout << "���յ��ͻ���" << (int)_cSock << "����Ϣ��username = " << login->userName << "\t" << "userpassword = " << login->PassWord << "\t"
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
		std::cout << "���յ��ͻ���"  << (int)_cSock << "����Ϣ��username = " << logout->userName << "\t"
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
	while (true)
	{
		//�������׽���
		fd_set fdRead;//��������socket������
		fd_set fdWrite;
		fd_set fdExcep;

		//������
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExcep);

		//��socket���뼯��
		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExcep);

		for (size_t n = 0; n < g_clients.size(); n++)
		{
			FD_SET(g_clients[n], &fdRead);
		}

		timeval t = { 1,0 };
		//����ʱ�����timeval��select��������ɷ�����
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExcep, &t);
		if (ret < 0)
		{
			std::cout << "select���������" << std::endl;
		}

		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			//�ȴ����ܿͻ������� accept
			sockaddr_in ClientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;
			_cSock = accept(_sock, (sockaddr*)&ClientAddr, &nAddrLen);
			if (_cSock == INVALID_SOCKET)
			{
				std::cout << "���ܵ���Ч�Ŀͻ�������!" << std::endl;
			}
			else
			{
				g_clients.push_back(_cSock);
				//��ͻ��˷��͹㲥
				for (size_t n = 0; n < g_clients.size(); n++)
				{
					NewClientJoin ncj;
					send(g_clients[n], (const char*)&ncj, sizeof(NewClientJoin), 0);
				}
				std::cout << "���յ��µĿͻ���IP:" << inet_ntoa(ClientAddr.sin_addr) << "\t"
					<< "socket��" << (int)_cSock << std::endl;
			}
		}
		//��������
		for (size_t n = 0; n < fdRead.fd_count; n++)
		{
			if (processor(fdRead.fd_array[n]) == -1)
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}
		//std::cout << "���д�������ҵ��" << std::endl;
	}
	//�ر�socket
	for (int n = 0; n < g_clients.size(); n++)
	{
		closesocket(g_clients[n]);
	}
	//�ر�Socket
	closesocket(_sock);
	WSACleanup();
	system("pause");
	return 0;
}