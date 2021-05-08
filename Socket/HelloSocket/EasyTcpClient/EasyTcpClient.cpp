#include "EasyTcpClient.h"

int EasyTcpClient::InitSocket()
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
		printf("socket %d creat errer\n", _sock);
	}
	else
	{
		// printf("socket creat success\n");
	}

	return _sock;
}

int EasyTcpClient::Connect(const char *ip, unsigned short port)
{
	if (_sock == INVALID_SOCKET)
	{
		InitSocket();
	}

	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_sin.sin_addr.S_addr = inet_addr(ip);
#endif // _WIN32
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (ret == SOCKET_ERROR)
	{
		printf("socket : %d connect error\n", _sock);
	}
	else
	{
		_isConnect = true;
		// printf("socket : %d connect success\n", _sock);
	}
	return ret;
}

void EasyTcpClient::Close()
{
	if (_sock == INVALID_SOCKET)
	{
		return;
	}

#ifdef _WIN32
	closesocket(_sock);
	WSACleanup(); // 和WSAStartup匹配
#else
	close(_sock);
#endif

	_isConnect = false;
}

bool EasyTcpClient::OnRun()
{
	if(!IsRun())
	{
		return false;
	}
	// 伯克利 socket
	fd_set fdRead;
	FD_ZERO(&fdRead);
	FD_SET(_sock, &fdRead);

	timeval t = { 1, 0 };
	// 第一个参数nfds是一个整数值，指fd_set集合中所有描述符(socket)的范围(即最大socket+1)
	// windows中不需要
	int ret = select(_sock + 1, &fdRead, 0, 0, &t);
	if (ret < 0)
	{
		printf("ret < 0, select exit\n");
		Close();
		return false;
	}

	if (FD_ISSET(_sock, &fdRead))
	{
		FD_CLR(_sock, &fdRead);
		// 3 向服务器发送数据
		if (RecvData() == -1)
		{
			printf("msg send == -1, select exit.\n");
			return false;
		}
	}

	return true;
}

int EasyTcpClient::RecvData()
{
	// 3 接受服务端数据
	// char szRecv[4096] = {};
	int recvLen = (int)recv(_sock, _szRecv, RECV_BUFF_SIZE, 0);
	//printf("recvLen = %d", recvLen);
	//DataHeader* header = (DataHeader*)_szRecv;
	if (recvLen <= 0)
	{
		printf("client exit\n");
		return -1;
	}
	memcpy(_szMsgBuf + _lastPos, _szRecv, recvLen);
	_lastPos += recvLen;

	// 判断消息缓冲区数据长度是否大于消息头
	// 就可以知道当前消息的长度
	while (_lastPos >= sizeof(DataHeader))
	{
		DataHeader* header = (DataHeader*)_szMsgBuf;
		if (_lastPos >= header->dataLength)
		{
			if (header->dataLength <= 0)
			{
				memcpy(_szMsgBuf, "", 1);
				break;
			}

			// 剩余消息缓冲区长度
			int size = _lastPos - header->dataLength;
			OnNetMsg(header);
			// 删掉二级缓冲区已处理的消息
			if (size > 0)
			{
				memcpy(_szMsgBuf, _szMsgBuf + header->dataLength, size);
			}
			else
			{
				// 没有消息
				memcpy(_szMsgBuf, _szMsgBuf + header->dataLength, 1);
			}
			_lastPos = size;
		}
		else
		{
			// 剩余数据不足一条完整消息
			break;
		}
	}

	if (_lastPos > 0)
	{

	}

	return 0;
}

void EasyTcpClient::OnNetMsg(DataHeader *header)
{
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		LoginResult* inRet = (LoginResult*)header;
		printf("recv server data. length : %d, cmd : %d\n", inRet->dataLength, inRet->cmd);
		break;
	}
	case CMD_LOGOUT_RESULT:
	{
		LogoutResult* outRet = (LogoutResult*)header;
		printf("recv server data : length : %d, cmd : %d\n", outRet->dataLength, outRet->cmd);
		break;
	}
	case CMD_NEW_USER_JOIN:
	{
		NewUserJoin* userJoin = (NewUserJoin*)header;
		printf("recv server data : length : %d, cmd : %d\n", userJoin->dataLength, userJoin->cmd);
		break;
	}
	case CMD_ERROR:
	{
		Error* error = (Error*)header;
		printf("recv server data : length : %d, cmd : %d\n", error->dataLength, error->cmd);
		break;
	}
	default:
		printf("error cmd.\n");
	}

}

int EasyTcpClient::SendData(const char* data, int length)
{
	int ret = SOCKET_ERROR;
	if (IsRun() && data != NULL)
	{
		ret = send(_sock, data, length, 0);
	}

	if (ret == SOCKET_ERROR)
	{
		Close();
	}

	return ret;
}
