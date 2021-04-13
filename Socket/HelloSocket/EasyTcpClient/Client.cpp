#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data); // ����socket
	// TODO ��д�����̴���
	// 1 ����һ��Socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock == INVALID_SOCKET)
	{
		printf("creat errer\n");
	}
	// 2 ���ӷ���� connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("connect error\n");
	}
	else
	{
		printf("connect success\n");
	}

	char cmdBuf[128] = {};
	while (true)
	{
		// 3 ���������������
		scanf_s("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("printf 'exit' cmd");
			break;
		}
		else
		{
			send(_sock, cmdBuf, strlen(cmdBuf + 1), 0);
		}

		// 5 ���ܷ�������Ϣ recv
		char recvBuf[256] = {};
		int recvLen = recv(_sock, recvBuf, 256, 0);
		if (recvLen <= 0)
		{
			printf("recv error\n");
		}
		else
		{
			printf("recv datas : %s \n", recvBuf);
		}
	}
	
	// 5 �Ͽ����� closesocket
	closesocket(_sock);

	WSACleanup(); // ��WSAStartupƥ��
	printf("client exit");
	getchar();
	return 0;
}