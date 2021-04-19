#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <thread>
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

int Process(SOCKET _cSock)
{
	// 3 接受服务端数据
	char szRecv[4096] = {};
	int recvLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (recvLen <= 0)
	{
		printf("client exit\n");
		return -1;
	}

	// 4 处理请求
	// send 向服务端发送数据
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LoginResult *inRet = (LoginResult *)szRecv;
		printf("recv server data. length : %d, cmd : %d\n", inRet->dataLength, inRet->cmd);
		return 0;
	}
	case CMD_LOGOUT_RESULT:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LogoutResult* outRet = (LogoutResult*)szRecv;
		printf("recv server data : length : %d, cmd : %d\n", outRet->dataLength, outRet->cmd);
		return 0;
	}
	case CMD_NEW_USER_JOIN:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		NewUserJoin* userJoin = (NewUserJoin*)szRecv;
		printf("recv server data : length : %d, cmd : %d\n", userJoin->dataLength, userJoin->cmd);
		return 0;
	}
	default:
		printf("error cmd.\n");
	}

	return -1;
}

bool G_ThreadFlag = true;
void CmdThread(SOCKET _sock)
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf_s("%s", cmdBuf, 256);
		if (0 == strcmp("exit", cmdBuf))
		{
			G_ThreadFlag = false;
			printf("thread exit\n");
			return;
		}
		else if (0 == strcmp("login", cmdBuf))
		{
			Login login;
			strcpy_s(login.userName, "CJC");
			strcpy_s(login.PassWord, "123456");
			send(_sock, (const char*)&login, sizeof(Login), 0);
		}
		else if (0 == strcmp("logout", cmdBuf))
		{
			Logout logout;
			strcpy_s(logout.userName, "CJC");
			send(_sock, (const char*)&logout, sizeof(Logout), 0);
		}
	}
}

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

	// 启动线程
	std::thread t1(CmdThread, _sock);
	t1.detach();

	while (G_ThreadFlag)
	{
		// 伯克利 socket
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(_sock, &fdRead);

		timeval t = { 1, 0 };
		// 第一个参数nfds是一个整数值，指fd_set集合中所有描述符(socket)的范围(即最大socket+1)
		// windows中不需要
		int ret = select(_sock + 1, &fdRead, 0, 0, &t);
		if (ret < 0)
		{
			printf("ret < 0, select exit\n");
			break;
		}

		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			// 3 向服务器发送数据
			if (Process(_sock) == -1)
			{
				printf("msg send == -1, select exit.\n");
				break;
			}
		}

		//printf("client idle.\n");
		//Sleep(1000);
	}
	
	// 5 断开连接 closesocket
	closesocket(_sock);

	WSACleanup(); // 和WSAStartup匹配
	printf("client exit");
	getchar();
	return 0;
}