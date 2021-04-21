#ifndef _EasyTcpClient_hpp_
#define _EasyTcpClient_hpp_

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd>
#include <arpa/inet.h>
#include <string.h>
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(0)
#define SOCKET_ERROR (-1)
#endif

#include <stdio.h>
#include <thread>

#include "DataDef.hpp"

class EasyTcpClient
{
public:
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
	}
	virtual ~EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
	}

public:
	// ����socket
	int InitSocket();

	// ����socket
	int Connect(const char* ip, unsigned short port);

	// �ر�socket
	void Close();

	// ��������
	int SendData(const char* data, int length);

	// ��������
	int RecvData();

	// ����������Ϣ
	bool OnRun();

	bool IsRun() { return _sock != INVALID_SOCKET; }
	void OnNetMsg(DataHeader* header);
private:
	SOCKET _sock;
};

int EasyTcpClient::InitSocket()
{
	if (_sock != INVALID_SOCKET)
	{
		printf("_sock exist, close and new socket\n");
		Close();
	}
#ifdef _WIN32
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data); // ����socket
#endif
	// TODO ��д�����̴���
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_sock == INVALID_SOCKET)
	{
		printf("socket creat errer\n");
	}
	else
	{
		printf("socket creat success\n");
	}

	return _sock;
}

int EasyTcpClient::Connect(const char *ip, unsigned short port)
{
	if (_sock == INVALID_SOCKET)
	{
		InitSocket();
	}

	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_sin.sin_addr.S_addr = inet_addr(ip);
#endif // _WIN32
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR)
	{
		printf("connect error\n");
	}
	else
	{
		printf("connect success\n");
	}
	return ret;
}

void EasyTcpClient::Close()
{
	if (_sock == INVALID_SOCKET)
	{
		return;
	}

#ifdef _WIN32
	closesocket(_sock);
	WSACleanup(); // ��WSAStartupƥ��
#else
	close(_sock);
#endif
}

bool EasyTcpClient::OnRun()
{
	if(!IsRun())
	{
		return false;
	}
	// ������ socket
	fd_set fdRead;
	FD_ZERO(&fdRead);
	FD_SET(_sock, &fdRead);

	timeval t = { 1, 0 };
	// ��һ������nfds��һ������ֵ��ָfd_set����������������(socket)�ķ�Χ(�����socket+1)
	// windows�в���Ҫ
	int ret = select(_sock + 1, &fdRead, 0, 0, &t);
	if (ret < 0)
	{
		printf("ret < 0, select exit\n");
		return false;
	}

	if (FD_ISSET(_sock, &fdRead))
	{
		FD_CLR(_sock, &fdRead);
		// 3 ���������������
		if (RecvData() == -1)
		{
			printf("msg send == -1, select exit.\n");
			return false;
		}
	}

	return true;
}

int EasyTcpClient::RecvData()
{
	// 3 ���ܷ��������
	char szRecv[4096] = {};
	int recvLen = (int)recv(_sock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (recvLen <= 0)
	{
		printf("client exit\n");
		return -1;
	}

	recv(_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
	OnNetMsg(header);

	return 0;
}

void EasyTcpClient::OnNetMsg(DataHeader *header)
{
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		LoginResult* inRet = (LoginResult*)header;
		printf("recv server data. length : %d, cmd : %d\n", inRet->dataLength, inRet->cmd);
		break;
	}
	case CMD_LOGOUT_RESULT:
	{
		LogoutResult* outRet = (LogoutResult*)header;
		printf("recv server data : length : %d, cmd : %d\n", outRet->dataLength, outRet->cmd);
		break;
	}
	case CMD_NEW_USER_JOIN:
	{
		NewUserJoin* userJoin = (NewUserJoin*)header;
		printf("recv server data : length : %d, cmd : %d\n", userJoin->dataLength, userJoin->cmd);
		break;
	}
	default:
		printf("error cmd.\n");
	}

}

int EasyTcpClient::SendData(const char* data, int length)
{
	if (IsRun() && data != NULL)
	{
		return send(_sock, data, length, 0);
	}

	return SOCKET_ERROR;
}


#endif // !_EasyTcpClient_hpp_
