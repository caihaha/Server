#pragma once
#ifndef _CELL_SERVER_H_
#define _CELL_SERVER_H_

#include <map>
#include <vector>

#include "ServerDefs.h"
#include "CELLClient.h"
#include "INetEvent.h"


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

		_oldTime = 0;
	}

	~CellServer()
	{
		Close();
		_sock = INVALID_SOCKET;
	}

	void Start();

	// ����������Ϣ
	bool OnRun();
	bool IsRun() { return _sock != INVALID_SOCKET; }

	// ��������
	int RecvData(CellClient* client);

	// ��Ӧ������Ϣ
	void OnNetMsg(CellClient* client, DataHeader* header);

	// �ر�socket
	void Close();

	void AddClientBuff(CellClient* client);

	void ClearClientBuff();

	size_t GetClientSize();

	void SetEventObj(INetEvent* event);

	void AddSendTask(CellClient* client, DataHeader* header);
private:
	CellServer() {}

	void AddClientFromBuff();

	void ReadData(fd_set fdRead);

	void CheckTime();
private:
	SOCKET _sock;
	std::map<SOCKET, CellClient*> _sock2Clients;
	// �������
	std::vector<CellClient*> _clientBuff;

	std::mutex _mutex;
	std::thread _thread;
	CELLTimestamp _tTime;
	INetEvent* _netEvent;

	fd_set _clientFdRead;
	bool _isFdReadChange;

	CellTaskServer _taskServer;
	time_t _oldTime;
};

#pragma endregion

#endif // !_CELL_SERVER_H_
