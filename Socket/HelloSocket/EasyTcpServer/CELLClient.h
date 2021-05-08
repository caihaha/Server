#pragma once

#ifndef _CELL_CLIENT_H_
#define _CELL_CLIENT_H_

#include "ServerDefs.h"

#pragma region CellClient
class CellClient
{
public:
	CellClient(const SOCKET sockfd)
	{
		_sockfd = sockfd;
		memset(_szMsgBuf, 0, sizeof(_szMsgBuf));
		_lastPos = 0;

		memset(_szSendMsgBuf, 0, sizeof(_szSendMsgBuf));
		_lastSendPos = 0;
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
		if (data == NULL)
		{
			return SOCKET_ERROR;
		}

		int ret = SOCKET_ERROR;
		while (_lastSendPos + length >= SEND_BUFF_SIZE)
		{
			const int nCpyLen = SEND_BUFF_SIZE - _lastSendPos;
			memcpy(_szSendMsgBuf + _lastSendPos, data, nCpyLen);
			data += nCpyLen;
			length -= nCpyLen;
			_lastSendPos = 0;

			ret = send(_sockfd, _szSendMsgBuf, SEND_BUFF_SIZE, 0);
			if (SOCKET_ERROR == ret)
			{
				return ret;
			}
		}

		if (length > 0)
		{
			memcpy(_szSendMsgBuf + _lastSendPos, data, length);
			_lastSendPos += length;
			int i = 0;
		}
		// ret = send(_sockfd, data, length, 0);
		return ret;
	}
private:
	SOCKET _sockfd; // socket fd_set file desc set
	// 接收缓冲区
	char _szRecv[RECV_BUFF_SIZE] = {};
	// 消息接收缓冲区
	char _szMsgBuf[RECV_BUFF_SIZE] = {};

	int _lastPos = 0;

	// 消息发送缓冲区
	char _szSendMsgBuf[SEND_BUFF_SIZE] = {};

	int _lastSendPos = 0;
};
#pragma endregion

#endif // !_CELL_CLIENT_H_
