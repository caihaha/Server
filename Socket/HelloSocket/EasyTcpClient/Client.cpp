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
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR,
};

struct DataHeader
{
	short cmd;
	short dataLength;
};
// DataPacket
struct Login : public DataHeader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];
};

struct LoginResult : public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

struct Logout : public DataHeader
{
	Logout()
	{
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

struct LogoutResult : public DataHeader
{
	LogoutResult()
	{
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

struct Error : public DataHeader
{
	Error()
	{
		dataLength = sizeof(LogoutResult);
		cmd = CMD_ERROR;
	}
};

struct NewUserJoin : public DataHeader
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		result = 0;
	}
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
			Login login;
			strcpy_s(login.userName, "CJC");
			strcpy_s(login.PassWord, "123456");
			send(_sock, (char *)&login, sizeof(Login), 0);

			// 接受数据
			LoginResult retLogin = {};
			recv(_sock, (char*)&retLogin, sizeof(LoginResult), 0);
			printf("LoginResult %d\n", retLogin.result);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout;
			strcpy_s(logout.userName, "CJC");
			send(_sock, (char *)&logout, sizeof(Logout), 0);

			LogoutResult retLogout = {};
			recv(_sock, (char*)&retLogout, sizeof(LogoutResult), 0);
			printf("LogoutResult %d\n", retLogout.result);
		}
		else
		{
			Error error;
			send(_sock, (char*)&error, sizeof(Error), 0);

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