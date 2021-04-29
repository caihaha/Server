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

#define RECV_BUFF_SIZE 10240
#define __CELL_SERVER_COUNT 4

#include <stdio.h>
#include<stdlib.h>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>

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

	bool operator==(const ClientSocket& var)
	{
		return var._sockfd == _sockfd;
	}
private:
	SOCKET _sockfd; // socket fd_set file desc set
	// ���ջ�����
	char _szRecv[RECV_BUFF_SIZE] = {};
	// ����������
	char _szMsgBuf[RECV_BUFF_SIZE * 10] = {};

	int _lastPos = 0;
};
#pragma endregion

#pragma region CellServer
class CellServer
{
public:
	CellServer(const SOCKET sock)
	{
		_sock = sock;
		_thread = NULL;
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
	int RecvData(ClientSocket* client);

	// ��Ӧ������Ϣ
	void OnNetMsg(SOCKET sock, DataHeader* header);

	// �ر�socket
	void Close();

	void AddClientBuff(ClientSocket* client);

	void ClearClientBuff();

	size_t GetClientSize();

	int GetRecvCount();

	void SetRecvCount(int count);
private:
	CellServer() {}

	void AddClientFromBuff();
private:
	SOCKET _sock;
	std::vector<ClientSocket*> _clients;
	// �������
	std::vector<ClientSocket*> _clientBuff;

	std::mutex _mutex;
	std::thread* _thread;
	static int _recvCount;
	CELLTimestamp _tTime;
};

#pragma endregion

#pragma region EasyTcpServer
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
	void Start();

	// ���ܿͻ�������
	int Accept();

	// �ر�socket
	void Close();

	// ��������
	int SendData(SOCKET _cSock, const char* data, int length);
	void SendData2All(const char* data, int length);

	// ��������
	// int RecvData(ClientSocket* client);

	// ����������Ϣ
	bool OnRun();

	bool IsRun() { return _sock != INVALID_SOCKET; }

	// ��Ӧ������Ϣ
	void Time4Msg();

private:

	void AddCellServer(ClientSocket* client);

	CellServer* GetMinClientCellServer();
private:
	SOCKET _sock;
	std::vector<ClientSocket*> _clients;
	std::vector<CellServer*> _cellServers;
	CELLTimestamp _tTime;
};
#pragma endregion

#endif // !_EasyTcpServer_hpp_

