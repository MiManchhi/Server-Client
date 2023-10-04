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
	_sin.sin_addr.S_un.S_addr = inet_addr("10.180.46.33");
	if (connect(_cSock, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		std::cout << "服务器连接失败！" << std::endl;
	}
	else
	{
		std::cout << "服务器连接成功!" << std::endl;
	}
	//发送请求
	while (true)
	{
		char msgBuf[256] = {};
		std::cout << "请输入请求：" << std::endl;
		std::cin >> msgBuf;
		//处理请求
		if (0 == strcmp(msgBuf, "exit"))
		{
			std::cout << "客户端退出！" << std::endl;
			break;
		}
		if (0 == strcmp(msgBuf, "login"))
		{
			Login login = {"张三","abcdef"};
			DataHeader header = {sizeof(Login),CMD_LOGIN};
			//发送请求
			send(_cSock, (const char*)&header, sizeof(DataHeader), 0);
			send(_cSock, (const char*)&login, sizeof(Login), 0);
			//接收信息
			LoginResult loginret = {};
			DataHeader headerret = {};
			recv(_cSock, (char*)&headerret, sizeof(DataHeader), 0);
			recv(_cSock, (char*)&loginret, sizeof(LoginResult), 0);
			std::cout << "LoginResult ：" << loginret.result << std::endl;
		}
		if (0 == strcmp(msgBuf, "logout"))
		{
			Logout logout = { "张三" };
			DataHeader header = { sizeof(Logout),CMD_LOGOUT };
			//发送请求
			send(_cSock, (const char*)&header, sizeof(DataHeader), 0);
			send(_cSock, (const char*)&logout, sizeof(Logout), 0);
			//接收信息
			LogoutResult logoutret = {};
			DataHeader headerret = {};
			recv(_cSock, (char*)&headerret, sizeof(DataHeader), 0);
			recv(_cSock, (char*)&logoutret, sizeof(LogoutResult), 0);
			std::cout << "LogoutResult : " << logoutret.result << std::endl;
		}
		if(0 != strcmp(msgBuf, "logout") && 0 != strcmp(msgBuf, "login"))
		{
			std::cout << "不支持的命令，请重新输入！" << std::endl;
		}
	}
	//关闭套接字
	closesocket(_cSock);
	//关闭Socket
	WSACleanup();
	system("pause");
	return 0;
}