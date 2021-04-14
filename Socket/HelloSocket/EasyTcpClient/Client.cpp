#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

enum CMD
{
	CMD_LOGIN,
	CMD_LOGOUT,
};

struct DataHeader
{
	short cmd;
	short dataLength;
};
// DataPacket
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
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data); // 启动socket
	// TODO 编写网络编程代码
	// 1 建立一个Socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock == INVALID_SOCKET)
	{
		printf("creat errer\n");
	}
	// 2 连接服务端 connect
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

	while (true)
	{
		// 3 向服务器发送数据
		char cmdBuf[128] = {};
		scanf_s("%s", cmdBuf, 128);

		if (0 == strcmp(cmdBuf, "exit"))
		{
			printf("printf 'exit' cmd\n");
			break;
		}
		else if (0 == strcmp(cmdBuf,"login"))
		{
			DataHeader dh = { CMD_LOGIN, sizeof(Login) };
			send(_sock, (char*)&dh, sizeof(DataHeader), 0);
			Login login = { "CJC", "123456" };
			send(_sock, (char *)&login, sizeof(Login), 0);

			// 接受数据
			DataHeader retHeader = {};
			LoginResult retLogin = {};
			recv(_sock, (char *)&retHeader, sizeof(DataHeader), 0);
			recv(_sock, (char*)&retHeader, sizeof(LoginResult), 0);
			printf("LoginResult %d\n", retLogin.result);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout = { "CJC"};
			DataHeader dh = { CMD_LOGOUT, sizeof(Logout) };
			send(_sock, (char *)&dh, sizeof(DataHeader), 0);
			send(_sock, (char *)&logout, sizeof(Login), 0);

			DataHeader retHeader = {};
			LogoutResult retLogout = {};
			recv(_sock, (char*)&retHeader, sizeof(DataHeader), 0);
			recv(_sock, (char*)&retHeader, sizeof(LogoutResult), 0);
			printf("LogoutResult %d\n", retLogout.result);
		}
		else
		{
			printf("cmd not exist. please input again:\n");
		}

		// 4 接受服务器信息 recv
		//char recvBuf[128] = {};
		//int recvLen = recv(_sock, recvBuf, 128, 0);
		//if (recvLen <= 0)
		//{
		//	printf("recv error\n");
		//}
	}
	
	// 5 断开连接 closesocket
	closesocket(_sock);

	WSACleanup(); // 和WSAStartup匹配
	printf("client exit");
	getchar();
	return 0;
}