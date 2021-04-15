#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <windows.h>
#include <stdio.h>
#include <cstring>
#include <vector>

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

std::vector<SOCKET>g_clients;

int Process(SOCKET _cSock)
{
	// 5 ���ܿͻ�������
	char szRecv[4096] = {};
	int recvLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (recvLen <= 0)
	{
		printf("client exit\n");
		return -1;
	}
	printf("recv data, cmd : %d, length : %d\n", header->cmd, header->dataLength);

	// 6 ��������
	// send ��ͻ��˷�������
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
	

	while (true)
	{
		// ������ socket
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);

		// ��һ������nfds��һ������ֵ��ָfd_set����������������(socket)�ķ�Χ(�����socket+1)
		// windows�в���Ҫ
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, NULL);
		if (ret < 0)
		{
			printf("select exit\n");
			break;
		}

		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);

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
			g_clients.push_back(_cSock);
		}

		for (size_t i = 0; i < fdRead.fd_count; ++i)
		{
			if (-1 == Process(fdRead.fd_array[i]))
			{
				auto iter = std::find(g_clients.begin(), g_clients.end(), fdRead.fd_array[i]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}
	}

	for (int i = (int)g_clients.size() - 1; i >= 0; --i)
	{
		closesocket(g_clients[i]);
	}
	
	// �ر��׽���closesocket;
	closesocket(_sock);

	WSACleanup(); // ��WSAStartupƥ��
	printf("server exit");
	getchar();
	return 0;
}