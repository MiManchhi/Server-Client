#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<WinSock2.h>
#include<Windows.h>
#include<iostream>
#pragma comment (lib,"ws2_32.lib")

struct DataPackage
{
	int age;
	char name[32];
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
		if (0 == strcmp(msgBuf, "exit"))
		{
			std::cout << "客户端退出！" << std::endl;
			break;
		}
		send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		//接收服务器信息 recv
		char recvBuf[256] = {};
		int nlen = recv(_cSock, recvBuf, 256, 0);
		if (nlen > 0)
		{
			DataPackage* dp = (DataPackage*)recvBuf;
			std::cout << "接收到信息：" << "name :" << dp->name << "\t" << "age :" << dp->age << std::endl;
		}
	}
	//关闭套接字
	closesocket(_cSock);
	//关闭Socket
	WSACleanup();
	system("pause");
	return 0;
}