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
	_sin.sin_addr.S_un.S_addr = inet_addr("10.180.50.74");
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
			Login login;
			strcpy((login.userName), "张三");
			strcpy((login.PassWord), "123456");
			//发送请求
			send(_cSock, (const char*)&login, sizeof(Login), 0);
			//接收信息
			LoginResult loginret;
			recv(_cSock, (char*)&loginret, sizeof(LoginResult), 0);
			std::cout << "username = " << login.userName << "\t" << "userpassword = " << login.PassWord << "\t"
				<< "LoginResult ：" << loginret.result << std::endl;
		}
		if (0 == strcmp(msgBuf, "logout"))
		{
			Logout logout;
			strcpy((logout.userName), "张三");
			//发送请求
			send(_cSock, (const char*)&logout, sizeof(Logout), 0);
			//接收信息
			LogoutResult logoutret;
			recv(_cSock, (char*)&logoutret, sizeof(LogoutResult), 0);
			std::cout << "username = " << logout.userName << "\t" << "LogoutResult : " << logoutret.result << std::endl;
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