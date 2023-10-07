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

std::vector<SOCKET> g_clients;

int DoProcess(SOCKET _cSock)
{

		//缓冲区
		char SZrecv[4096] = {};
		int nlen = recv(_cSock, SZrecv, sizeof(DataHeader), 0);
		DataHeader* header = (DataHeader*)SZrecv;
		if (nlen <= 0)
		{
			std::cout << "客户端已退出！" << std::endl;
			return -1;
		}
		//处理请求
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			recv(_cSock, SZrecv + sizeof(DataHeader), header->datalength - sizeof(DataHeader), 0);
			Login* login = (Login*)SZrecv;
			std::cout << "接收到信息：username = " << login->userName << "\t" << "userpassword = " << login->PassWord << "\t"
				<< "datalength = " << login->datalength << std::endl;
			//忽略判断用户名和密码错误的过程
			LoginResult ret;
			send(_cSock, (const char*)&ret, sizeof(LoginResult), 0);
		}
		break;
		case CMD_LOGOUT:
		{
			recv(_cSock, SZrecv + sizeof(DataHeader), header->datalength - sizeof(DataHeader), 0);
			Logout* logout = (Logout*)SZrecv;
			std::cout << "接收到信息：username = " << logout->userName << "\t"
				<< "datalength = " << logout->datalength << std::endl;
			//忽略判断用户名和密码错误的过程
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
	//启动Socket
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
	//建立一个Socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//绑定接受连接客户端的端口 bind 
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567); //host to net unsigned short
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;// 默认IP地址（所有都可以访问//inet_addr("10.180.44.214");
	if (bind(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR)  //绑定可能失败,判断
	{
		std::cout << "网络端口绑定失败！" << std::endl;
	}
	else
	{
		std::cout << "网络端口绑定成功！" << std::endl;
	}
	//监听网络端口 listen
	if (listen(_sock, 5) == SOCKET_ERROR)
	{
		std::cout << "网络端口监听失败！" << std::endl;
	}
	else
	{
		std::cout << "网络端口监听成功！" << std::endl;
	}
	while (true)
	{
		//伯克利socket
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExcep;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExcep);

		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExcep);

		for (int n = (int)g_clients.size()-1; n >= 0; n--)
		{
			FD_SET(_sock, g_clients[n]);
		}

		//nfds是一个整数值，是fd_set集合中所有描述符（socket）的范围，而不是数量
		//即是所有文件描述符+1，在Windows中这个参数可以忽略，可以填0
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExcep, NULL);

		if (ret < 0)
		{
			std::cout << "socket任务结束" << std::endl;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);

			//等待接受客户端连接 accept
			sockaddr_in ClientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;
			//循环，接收多个
			_cSock = accept(_sock, (sockaddr*)&ClientAddr, &nAddrLen);
			if (_cSock == INVALID_SOCKET)
			{
				std::cout << "接受的无效的客户端连接!" << std::endl;
			}
			g_clients.push_back(_cSock);
			std::cout << "接收的新的客户端IP:" << inet_ntoa(ClientAddr.sin_addr) << "\t" << "socket：" << (int)_cSock << std::endl;
			//接收并且处理请求
			for (size_t n = 0;n < fdRead.fd_count; n++)
			{
				if (DoProcess(fdRead.fd_array[n]) == -1)
				{
					auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
					if (iter != g_clients.end())
					{
						g_clients.erase(iter);
					}
				}
			}
		}
	}
	//关闭Socket
	for (int n = 0; n < g_clients.size(); n++)
	{
		closesocket(g_clients[n]);
	}
	closesocket(_sock);
	WSACleanup();
	system("pause");
	return 0;
}