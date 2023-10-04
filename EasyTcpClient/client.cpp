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
		if (0 == strcmp(msgBuf, "exit"))
		{
			std::cout << "�ͻ����˳���" << std::endl;
			break;
		}
		send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		//���շ�������Ϣ recv
		char recvBuf[256] = {};
		int nlen = recv(_cSock, recvBuf, 256, 0);
		if (nlen > 0)
		{
			DataPackage* dp = (DataPackage*)recvBuf;
			std::cout << "���յ���Ϣ��" << "name :" << dp->name << "\t" << "age :" << dp->age << std::endl;
		}
	}
	//�ر��׽���
	closesocket(_cSock);
	//�ر�Socket
	WSACleanup();
	system("pause");
	return 0;
}