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
		_sock = INVALID_SOCKET;
	}
	~EasyTcpServer()
	{
		Close();
	}

public:
	// 建立socket
	int InitSocket();

	// 绑定端口
	int Bind(const char* ip, unsigned short port);

	// 监听
	int Listen(int linkCnt);

	// 接受客户端连接
	int Accept();

	// 关闭socket
	void Close();

	// 发送数据
	int SendData(SOCKET _cSock, const char* data, int length);
	void SendData2All(const char* data, int length);

	// 接受数据
	int RecvData(SOCKET _cSock);

	// 处理网络消息
	bool OnRun();

	bool IsRun() { return _sock != INVALID_SOCKET; }
	
	// 响应网络消息
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
	WSAStartup(ver, &data); // 启动socket
#endif
	// TODO 编写网络编程代码
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
		printf("connect error. port : %d\n", port);
	}
	else
	{
		printf("connect success. port : %d\n", port);
	}
	return ret;
}

int EasyTcpServer::Listen(int linkCnt)
{
	// 3 listen 监听网络端口
	int ret = listen(_sock, linkCnt) == SOCKET_ERROR;
	if (ret == SOCKET_ERROR)
	{
		printf("listen error. socket: %d \n", _sock);
	}
	else
	{
		printf("listen success, socket : %d\n", _sock);
	}
	return ret;
}

int EasyTcpServer::Accept()
{
	// 4 accept 等待接受客户端连接
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
#ifdef _WIN32
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
	_cSock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t*)&nAddrLen);
#endif // _WIN32

	if (INVALID_SOCKET == _cSock)
	{
		printf("client socket invalid\n");
	}
	else
	{
		NewUserJoin userJoin;
		SendData2All((const char*)&userJoin, userJoin.dataLength);
		_clients.push_back(_cSock);
		printf("New Client : socket = %d, IP = %s \n", _cSock, inet_ntoa(clientAddr.sin_addr));
	}

	return _sock;
}

bool EasyTcpServer::OnRun()
{
	// 伯克利 socket
	fd_set fdRead;
	fd_set fdWrite;
	fd_set fdExp;

	FD_ZERO(&fdRead);
	FD_ZERO(&fdWrite);
	FD_ZERO(&fdExp);

	FD_SET(_sock, &fdRead);
	FD_SET(_sock, &fdWrite);
	FD_SET(_sock, &fdExp);
	SOCKET maxSock = _sock;

	for (int i = _clients.size() - 1; i >= 0; --i)
	{
		FD_SET(_clients[i], &fdRead);
		if (_clients[i] > maxSock)
		{
			maxSock = _clients[i];
		}
	}

	timeval t = { 1, 0 };
	// 第一个参数nfds是一个整数值，指fd_set集合中所有描述符(socket)的范围(即最大socket+1)
	// windows中不需要
	int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);
	if (ret < 0)
	{
		printf("select exit\n");
		return false;
	}
	// printf("server idle.\n"); // 测试t不为0时，select不阻塞

	if (FD_ISSET(_sock, &fdRead))
	{
		FD_CLR(_sock, &fdRead);
		Accept();
	}

#ifdef _WIN32
	for (size_t i = 0; i < fdRead.fd_count; ++i)
	{
		if (-1 == RecvData(fdRead.fd_array[i]))
		{
			auto iter = std::find(_clients.begin(), _clients.end(), fdRead.fd_array[i]);
			if (iter != _clients.end())
			{
				_clients.erase(iter);
			}
		}
	}
#else
	for (int i = (int)g_clients.size() - 1; i >= 0; --i)
	{
		if (FD_ISSET(_clients[i], &fdRead))
		{
			if (-1 == Process(_clients[i]))
			{
				auto iter = _clients.begin() + i;
				if (iter != _clients.end())
				{
					_clients.rease(iter);
				}
			}
		}
	}
#endif // _WIN32
}

void EasyTcpServer::Close()
{
	if (_sock == INVALID_SOCKET)
	{
		return;
	}

#ifdef _WIN32
	for (int i = (int)_clients.size() - 1; i >= 0; --i)
	{
		closesocket(_clients[i]);
	}
	// 关闭套接字closesocket;
	closesocket(_sock);
	WSACleanup(); // 和WSAStartup匹配
#else
	for (int i = (int)_clients.size() - 1; i >= 0; --i)
	{
		close(_clients[i]);
	}
	close(_sock);
#endif
}

int EasyTcpServer::SendData(SOCKET _cSock, const char* data, int length)
{
	if (IsRun() && data != NULL)
	{
		return send(_sock, data, length, 0);
	}

	return SOCKET_ERROR;
}

void EasyTcpServer::SendData2All(const char* data, int length)
{
	if (IsRun() && data != NULL)
	{
		for (int i = (int)_clients.size() - 1; i >= 0; --i)
		{
			if (SendData(_clients[i], data, length) == SOCKET_ERROR)
			{
				printf("send data error. socket : %d", _clients[i]);
			}
		}
	}
}

int EasyTcpServer::RecvData(SOCKET _cSock)
{
	// 5 接受客户端数据
	char szRecv[4096] = {};
	int recvLen = (int)recv(_cSock, szRecv, 4096, 0);
	// DataHeader* header = (DataHeader*)szRecv;
	if (recvLen <= 0)
	{
		printf("client exit\n");
		return -1;
	}
	// printf("recv data, cmd : %d, length : %d\n", header->cmd, header->dataLength);
	
	// recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
	// OnNetMsg(header);
	
	return 0;
}

void EasyTcpServer::OnNetMsg(DataHeader* header)
{
	// 6 处理请求
	// send 向客户端发送数据
	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		LoginResult inRet;
		send(_sock, (char*)&inRet, sizeof(LoginResult), 0);
		return;
	}
	case CMD_LOGOUT:
	{
		LogoutResult outRet;
		send(_sock, (char*)&outRet, sizeof(LogoutResult), 0);
		return;
	}
	default:
	{
		DataHeader header = { 0, CMD_ERROR };
		send(_sock, (char*)&header, sizeof(header), 0);
		printf("error cmd.\n");
	}
	}
}


#endif // !_EasyTcpServer_hpp_