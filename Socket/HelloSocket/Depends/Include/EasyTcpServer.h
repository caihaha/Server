#pragma once
#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_

#include <stdio.h>
#include<stdlib.h>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>
#include <map>

#include "CELLDefs.h"
#include "CELLServer.h"
#include "CELLClient.h"
#include "INetEvent.h"
#include "CELLServer.cpp"

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

	// 接受数据
	// int RecvData(CellClient* client);

	// 处理网络消息
	bool OnRun();

	bool IsRun() { return _sock != INVALID_SOCKET; }

	// 响应网络消息
	void Time4Msg();

	virtual void OnJoin(CellClient* client);

	virtual void OnLeave(CellClient* client);

	virtual void OnNetMsg(CellClient* client, DataHeader* header, CellServer* cellServer);
private:

	void AddCellServer(CellClient* client);

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

