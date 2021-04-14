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
	WSAStartup(ver, &data); // ����socket
	// TODO ��д�����̴���
	// 1 ����һ��socket ����unit *
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// 2 �󶨶˿ں� bind
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
	// 3 listen ��������˿�
	if (listen(_sock, 5) == SOCKET_ERROR)
	{
		printf("listen error\n");
	}
	else
	{
		printf("listen success\n");
	}
	// 4 accept �ȴ����ܿͻ�������
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
		DataHeader header = {};
		// 5 ���ܿͻ�������
		int recvLen = recv(_cSock, (char *)&header, sizeof(DataHeader), 0);
		if (recvLen <= 0)
		{
			printf("client exit\n");
			break;
		}
		printf("recv data, cmd : %d, length : %d\n", header.cmd, header.dataLength);
		// 6 ��������
		// send ��ͻ��˷�������
		switch (header.cmd)
		{
		case CMD_LOGIN:
		{
			Login login = {};
			recv(_cSock, (char*)&login, sizeof(Login), 0);
			send(_cSock, (char*)&header, sizeof(DataHeader), 0);
			LoginResult inRet = { 1 };
			send(_cSock, (char*)&inRet, sizeof(LoginResult), 0);
			break;
		}
		case CMD_LOGOUT:
		{
			Logout logout = {};
			recv(_cSock, (char*)&logout, sizeof(Logout), 0);
			send(_cSock, (char*)&header, sizeof(DataHeader), 0);
			LogoutResult outRet = { 1 };
			send(_cSock, (char*)&outRet, sizeof(LogoutResult), 0);
			break;
		}
		default:
			printf("error cmd\n");
		}
	}
	
	// �ر��׽���closesocket;
	closesocket(_sock);

	WSACleanup(); // ��WSAStartupƥ��
	printf("server exit");
	getchar();
	return 0;
}