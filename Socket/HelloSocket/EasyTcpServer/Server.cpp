#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data); // 启动socket
	// TODO 编写网络编程代码
	// 1 建立一个socket 返回unit *
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 2 绑定端口号 bind
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567); // host to net unsigned shot
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (bind(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("bind error\n");
	}
	else
	{
		printf("bind success\n");
	}
	// 3 listen 监听网络端口
	if (listen(_sock, 5) == SOCKET_ERROR)
	{
		printf("listen error\n");
	}
	else
	{
		printf("listen success\n");
	}
	// 4 accept 等待接受客户端连接
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
	
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock)
	{
		printf("client socket invalid\n");
	}
	else
	{
		printf("accept client IP = %s\n", inet_ntoa(clientAddr.sin_addr));
	}
	char recvBuff[128] = {};
	while (true)
	{
		char msgBuf[256] = "";
		// 5 接受客户端数据
		int recvLen = recv(_cSock, recvBuff, 128, 0);
		if (recvLen <= 0)
		{
			printf("client exit\n");
			break;
		}
		// 6 处理请求
		if (0 == strcmp(recvBuff, "getName"))
		{
			strcpy_s(msgBuf, "I'm CJC.");
		}
		else if (0 == strcmp(recvBuff, "getAge"))
		{
			strcpy_s(msgBuf, "25");
		}
		else
		{
			strcpy_s(msgBuf, "???");
		}
		// send 向客户端发送数据
		send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
	}
	
	// 关闭套接字closesocket;
	closesocket(_sock);

	WSACleanup(); // 和WSAStartup匹配
	printf("server exit");
	getchar();
	return 0;
}