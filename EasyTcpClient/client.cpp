#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<WinSock2.h>
#include<Windows.h>
#include<iostream>
#include<thread>

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

int processor(SOCKET _cSock)
{
	//缓冲区
	char SZrecv[4096] = {};
	int nlen = recv(_cSock, SZrecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)SZrecv;
	if (nlen <= 0)
	{
		std::cout << "与服务器断开连接，任务结束！" << std::endl;
		return -1;
	}
	//处理请求
	switch (header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			recv(_cSock, SZrecv + sizeof(DataHeader), header->datalength - sizeof(DataHeader), 0);
			LoginResult *ret = (LoginResult*)SZrecv;
			std::cout << "接收到服务器信息：LOGIN_RESULT" << "\t" << "数据长度：" << ret->datalength <<
				std::endl;
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			recv(_cSock, SZrecv + sizeof(DataHeader), header->datalength - sizeof(DataHeader), 0);
			LogoutResult* ret = (LogoutResult*)SZrecv;
			std::cout << "接收到服务器信息：LOGOUT_RESULT" << "\t" << "数据长度：" << ret->datalength <<
				std::endl;
		}
		break;
		case CMD_NEW_CLIENT_JOIN:
		{
			recv(_cSock, SZrecv + sizeof(DataHeader), header->datalength - sizeof(DataHeader), 0);
			NewClientJoin* ret = (NewClientJoin*)SZrecv;
			std::cout << "接收到服务器信息：NEW_CLIENT_JOIN" << "\t" << "数据长度：" << ret->datalength <<
				std::endl;
		}
		break;
	}
}

bool g_ThreadRun = true;
void cmdthread(SOCKET _cSock)
{
	while (true)
	{
		char cmdBuf[256] = {};
		//std::cout << "请输入请求：" << std::endl;
		std::cin >> cmdBuf;
		if (strcmp(cmdBuf, "exit") == 0)
		{
			g_ThreadRun = false;
			std::cout << "任务结束，已退出！" << std::endl;
			break;
		}
		if (strcmp(cmdBuf, "login") == 0)
		{
			Login login;
			strcpy(login.userName, "张三");
			strcpy(login.PassWord, "123456");
			send(_cSock, (const char*)&login, sizeof(Login), 0);
		}
		if (strcmp(cmdBuf, "logout") == 0)
		{
			Logout logout;
			strcpy(logout.userName, "张三");
			send(_cSock, (const char*)&logout, sizeof(Logout), 0);
		}
		if (strcmp(cmdBuf, "login") != 0 && strcmp(cmdBuf, "logout") != 0)
		{
			std::cout << "不支持的命令！" << std::endl;
		}
	}
}

int main()
{
	//启动Socket
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);
	//建立socket
	SOCKET _cSock = socket(AF_INET, SOCK_STREAM, 0);
	if (_cSock == INVALID_SOCKET)
	{
		std::cout << "套接字建立失败！" << std::endl;
	}
	else
	{
		std::cout << "套接字建立成功!" << std::endl;
	}
	//连接服务器 connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("10.180.58.102");
	if (connect(_cSock, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		std::cout << "服务器连接失败！" << std::endl;
	}
	else
	{
		std::cout << "服务器连接成功!" << std::endl;
	}
	//启动线程
	std::thread t1(cmdthread, _cSock);
	t1.detach();
	//发送请求
	while (g_ThreadRun)
	{
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(_cSock,&fdRead);

		timeval t = { 1,0 };

		int ret = select(_cSock, &fdRead, 0, 0, &t);
		if (ret < 0)
		{
			std::cout << "已退出！" << std::endl;
		}
		if (FD_ISSET(_cSock, &fdRead))
		{
			FD_CLR(_cSock, &fdRead);
			if (processor(_cSock) == -1)
			{
				break;
			}
		}
		//std::cout << "空闲处理其他业务" << std::endl;
	}
	//关闭套接字
	closesocket(_cSock);
	//关闭Socket
	WSACleanup();
	system("pause");
	return 0;
}