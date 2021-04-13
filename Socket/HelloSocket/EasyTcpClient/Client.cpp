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
	WSAStartup(ver, &data); // 启动socket
	// TODO 编写网络编程代码
	// 1 建立一个Socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock == INVALID_SOCKET)
	{
		printf("creat errer");
	}
	// 2 连接服务端 connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("connect error");
	}
	// 3 接受服务器信息 recv
	char recvBuff[256] = {};
	int recvLen = recv(_sock, recvBuff, 256, 0);
	if (recvLen <= 0)
	{
		printf("recv error");
	}
	else
	{
		printf("recv datas : %s \n", recvBuff);
	}
	// 4 断开连接 closesocket
	closesocket(_sock);

	WSACleanup(); // 和WSAStartup匹配
	return 0;
}