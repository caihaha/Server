#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

enum CMD
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT_RESULT,
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

	while (true)
	{
		// 5 接受客户端数据
		char szRecv[4096] = {};
		int recvLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
		DataHeader* header = (DataHeader*)szRecv;
		if (recvLen <= 0)
		{
			printf("client exit\n");
			break;
		}
		printf("recv data, cmd : %d, length : %d\n", header->cmd, header->dataLength);
		
		// 6 处理请求
		// send 向客户端发送数据
		switch (header->cmd)
		{
			
		case CMD_LOGIN:
		{
			Login login = {};
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LoginResult inRet;
			send(_cSock, (char*)&inRet, sizeof(LoginResult), 0);
			break;
		}
		case CMD_LOGOUT:
		{
			Logout logout = {};
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LogoutResult outRet;
			send(_cSock, (char*)&outRet, sizeof(LogoutResult), 0);
			break;
		}
		default:
			printf("error cmd\n");
		}
	}
	
	// 关闭套接字closesocket;
	closesocket(_sock);

	WSACleanup(); // 和WSAStartup匹配
	printf("server exit");
	getchar();
	return 0;
}