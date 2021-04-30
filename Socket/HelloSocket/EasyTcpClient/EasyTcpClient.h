#ifndef _EasyTcpClient_hpp_
#define _EasyTcpClient_hpp_

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
#include <thread>

#include "DataDef.hpp"
#include "CELLTimestamp.hpp"

#pragma region EasyTcpClient
class EasyTcpClient
{
public:
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
		_isConnect = false;
		_lastPos = 0;
	}
	virtual ~EasyTcpClient()
	{
		Close();
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

	bool IsRun() { return _sock != INVALID_SOCKET && _isConnect; }

	// ��Ӧ������Ϣ
	void OnNetMsg(DataHeader* header);
private:
	SOCKET _sock;
	// ���ջ�����
	char _szRecv[RECV_BUFF_SIZE] = {};
	// ����������
	char _szMsgBuf[RECV_BUFF_SIZE * 5] = {};

	int _lastPos;

	bool _isConnect;
};
#pragma endregion

#endif // !_EasyTcpClient_hpp_