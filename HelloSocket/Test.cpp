#include<WinSock2.h>
#include<Windows.h>

int main()
{
	//����һ��Socket
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver,&data);
	//�󶨽������ӿͻ��˵Ķ˿� bind

	//��������˿� listen

	//�ȴ����ܿͻ������� accept

	//��ͻ��˷������� send

	//�ر�Socket
	WSACleanup();
	return 0;

	//����socket
	//����socket
	//���ӷ�����
	//������Ϣ
	//�ر�socket
}