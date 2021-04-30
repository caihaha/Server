#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_

#ifdef _WIN32
#define FD_SETSIZE 1024
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

#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240
#endif // !RECV_BUFF_SIZE

#include <stdio.h>
#include<stdlib.h>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>
#include <map>

#include "DataDef.hpp"
#include "CELLTimestamp.hpp"

#pragma region ClientSocket
class ClientSocket
{
public:
	ClientSocket(const SOCKET sockfd)
	{
		_sockfd = sockfd;
		memset(_szMsgBuf, 0, sizeof(_szMsgBuf));
		_lastPos = 0;
	}

	const SOCKET GetSocketfd() const
	{
		return _sockfd;
	}

	char* RecvBuf()
	{
		return _szRecv;
	}

	char* MsgBuf()
	{
		return _szMsgBuf;
	}

	const int GetLastPos() const
	{
		return _lastPos;
	}

	void SetLastPos(const int pos)
	{
		_lastPos = pos;
	}

	int SendData(const char* data, int length)
	{
		if (data != NULL)
		{
			return send(_sockfd, data, length, 0);
		}

		return SOCKET_ERROR;
	}
private:
	SOCKET _sockfd; // socket fd_set file desc set
	// 接收缓冲区
	char _szRecv[RECV_BUFF_SIZE] = {};
	// 二级缓冲区
	char _szMsgBuf[RECV_BUFF_SIZE * 5] = {};

	int _lastPos = 0;
};
#pragma endregion

#pragma region INetEvent
// 事件处理
class INetEvent
{
public:
	INetEvent()
	{
	}
	virtual ~INetEvent()
	{
	}

	// 客户端加入事件
	virtual void OnJoin(ClientSocket* client) = 0;
	// 客户端离开事件
	virtual void OnLeave(ClientSocket* client) = 0;
	// 客户端消息时间
	virtual void OnNetMsg(ClientSocket* client, DataHeader* header) = 0;

private:

};
#pragma endregion

#pragma region CellServer
class CellServer
{
public:
	CellServer(const SOCKET sock)
	{
		_sock = sock;
		_netEvent = NULL;
		_isFdReadChange = false;
		FD_ZERO(&_clientFdRead);
	}

	~CellServer()
	{
		Close();
		_sock = INVALID_SOCKET;
	}

	void Start();

	// 处理网络消息
	bool OnRun();
	bool IsRun() { return _sock != INVALID_SOCKET; }

	// 接受数据
	int RecvData(ClientSocket* client);

	// 响应网络消息
	void OnNetMsg(ClientSocket* client, DataHeader* header);

	// 关闭socket
	void Close();

	void AddClientBuff(ClientSocket* client);

	void ClearClientBuff();

	size_t GetClientSize();

	void SetEventObj(INetEvent* event);
private:
	CellServer() {}

	void AddClientFromBuff();
private:
	SOCKET _sock;
	std::map<SOCKET, ClientSocket*> _sock2Clients;
	// 缓冲队列
	std::vector<ClientSocket*> _clientBuff;

	std::mutex _mutex;
	std::thread _thread;
	CELLTimestamp _tTime;
	INetEvent* _netEvent;

	fd_set _clientFdRead;
	bool _isFdReadChange;
};

#pragma endregion

#pragma region EasyTcpServer
class EasyTcpServer : public INetEvent
{
public:
	EasyTcpServer()
	{
		_sock = INVALID_SOCKET;
		_msgCount = 0;
		_clientCount = 0;
	}
	~EasyTcpServer()
	{
		Close();
		_sock = INVALID_SOCKET;
	}

public:
	// 建立socket
	int InitSocket();

	// 绑定端口
	int Bind(const char* ip, unsigned short port);

	// 监听
	int Listen(int linkCnt);

	// 开启多线程
	void Start(const int threadCpunt);

	// 接受客户端连接
	int Accept();

	// 关闭socket
	void Close();

	// 发送数据
	int SendData(SOCKET _cSock, const char* data, int length);

	// 接受数据
	// int RecvData(ClientSocket* client);

	// 处理网络消息
	bool OnRun();

	bool IsRun() { return _sock != INVALID_SOCKET; }

	// 响应网络消息
	void Time4Msg();

	virtual void OnJoin(ClientSocket* client);

	virtual void OnLeave(ClientSocket* client);

	virtual void OnNetMsg(ClientSocket* client, DataHeader* header);
private:

	void AddCellServer(ClientSocket* client);

	CellServer* GetMinClientCellServer();
private:
	SOCKET _sock;
	// 多线程处理客户端消息
	std::vector<CellServer*> _cellServers;
	CELLTimestamp _tTime;

protected:
	std::atomic_int _clientCount;
	std::atomic_int _msgCount;
};
#pragma endregion

#endif // !_EasyTcpServer_hpp_

