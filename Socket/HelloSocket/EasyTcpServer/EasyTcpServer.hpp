#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_

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
#include <vector>

#include "DataDef.hpp"

class EasyTcpServer
{
public:
	EasyTcpServer()
	{
	}
	~EasyTcpServer()
	{
	}

public:
	// ����socket
	int InitSocket();

	// �󶨶˿�
	int Bind(const char* ip, unsigned short port);

	// �ر�socket
	void Close();

	// ��������
	int SendData(DataHeader* header);

	// ��������
	int RecvData();

	// ����������Ϣ
	bool OnRun();

	bool IsRun() { return _sock != INVALID_SOCKET; }
	void OnNetMsg(DataHeader* header);

private:
	SOCKET _sock;
	std::vector<SOCKET> _clients;
};

int EasyTcpServer::InitSocket()
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

int EasyTcpServer::Bind(const char* ip, unsigned short port)
{
	if (_sock == INVALID_SOCKET)
	{
		InitSocket();
	}

	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
	if (ip == NULL)
	{
	#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = INADDR_ANY;
	#else
		_sin.sin_addr.S_addr = INADDR_ANY;
	#endif // _WIN32
	}
	else
	{
	#ifdef _WIN32
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
	#else
		_sin.sin_addr.S_addr = inet_addr(ip);
	#endif // _WIN32
	}

	int ret = bind(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
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

#endif // !_EasyTcpServer_hpp_