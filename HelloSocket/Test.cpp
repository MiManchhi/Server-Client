#include<WinSock2.h>
#include<Windows.h>

int main()
{
	//建立一个Socket
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver,&data);
	//绑定接受连接客户端的端口 bind

	//监听网络端口 listen

	//等待接受客户端连接 accept

	//向客户端发送数据 send

	//关闭Socket
	WSACleanup();
	return 0;

	//启动socket
	//建立socket
	//连接服务器
	//接收信息
	//关闭socket
}