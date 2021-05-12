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
	// ����socket
	int InitSocket();

	// �󶨶˿�
	int Bind(const char* ip, unsigned short port);

	// ����
	int Listen(int linkCnt);

	// �������߳�
	void Start(const int threadCpunt);

	// ���ܿͻ�������
	int Accept();

	// �ر�socket
	void Close();

	// ��������
	// int RecvData(CellClient* client);

	// ����������Ϣ
	bool OnRun();

	bool IsRun() { return _sock != INVALID_SOCKET; }

	// ��Ӧ������Ϣ
	void Time4Msg();

	virtual void OnJoin(CellClient* client);

	virtual void OnLeave(CellClient* client);

	virtual void OnNetMsg(CellClient* client, DataHeader* header, CellServer* cellServer);
private:

	void AddCellServer(CellClient* client);

	CellServer* GetMinClientCellServer();
private:
	SOCKET _sock;
	// ���̴߳���ͻ�����Ϣ
	std::vector<CellServer*> _cellServers;
	CELLTimestamp _tTime;

protected:
	std::atomic_int _clientCount;
	std::atomic_int _msgCount;
};
#pragma endregion

#endif // !_EasyTcpServer_hpp_

