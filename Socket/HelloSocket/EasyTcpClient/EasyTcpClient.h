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

#define RECV_BUFF_SIZE 10240

#include <stdio.h>
#include <thread>

#include "DataDef.hpp"

#pragma region EasyTcpClient
class EasyTcpClient
{
public:
	EasyTcpClient()
	{
		_sock = INVALID_SOCKET;
	}
	virtual ~EasyTcpClient()
	{
		Close();
	}

public:
	// 建立socket
	int InitSocket();

	// 连接socket
	int Connect(const char* ip, unsigned short port);

	// 关闭socket
	void Close();

	// 发送数据
	int SendData(const char* data, int length);

	// 接受数据
	int RecvData();

	// 处理网络消息
	bool OnRun();

	bool IsRun() { return _sock != INVALID_SOCKET; }

	// 响应网络消息
	void OnNetMsg(DataHeader* header);
private:
	SOCKET _sock;
	// 接收缓冲区
	char _szRecv[RECV_BUFF_SIZE] = {};
	// 二级缓冲区
	char _szMsgBuf[RECV_BUFF_SIZE * 10] = {};

	int _lastPos = 0;
};
#pragma endregion

#endif // !_EasyTcpClient_hpp_