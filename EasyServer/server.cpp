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
	std::cout << "接收的新的客户端IP:" << inet_ntoa(ClientAddr.sin_addr) << "\t" << "socket：" << (int)_cSock << std::endl;
	char recvBuf[256] = {};
	//接收请求
	while (true)
	{
		int nlen = recv(_cSock, recvBuf, 256, 0);
		if (nlen <= 0)
		{
			std::cout << "客户端已退出！";
			break;
		}
		//处理请求
		if (0 == strcmp(recvBuf, "ShowInfo"))
		{
			DataPackage dp = { 18,"张三" };
			send(_cSock, (const char*)&dp, sizeof(DataPackage), 0);
		}
		else
		{
			char msgBuf[] = "???";
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
	}
	//关闭Socket
	closesocket(_sock);

	WSACleanup();
	system("pause");
	return 0;
}