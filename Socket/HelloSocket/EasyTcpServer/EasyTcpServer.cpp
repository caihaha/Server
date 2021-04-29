#include "EasyTcpServer.h"

#pragma region CellServer
void CellServer::Start()
{
	_thread = std::thread(std::mem_fn(&CellServer::OnRun), this);
}

bool CellServer::OnRun()
{
	while (IsRun())
	{
		AddClientFromBuff();

		if (_clients.empty())
		{
			std::chrono::milliseconds t(1);
			std::this_thread::sleep_for(t);
			continue;
		}

		// 伯克利 socket
		fd_set fdRead;
		FD_ZERO(&fdRead);

		SOCKET maxSock = _sock;
		for (int i = _clients.size() - 1; i >= 0; --i)
		{
			SOCKET clientSocket = _clients[i]->GetSocketfd();
			FD_SET(clientSocket, &fdRead);
			if (clientSocket > maxSock)
			{
				maxSock = clientSocket;
			}
		}

		// timeval t = { 1, 0 };
		// 第一个参数nfds是一个整数值，指fd_set集合中所有描述符(socket)的范围(即最大socket+1)
		// windows中不需要
		int ret = select(maxSock + 1, &fdRead, NULL, NULL, NULL);
		if (ret < 0)
		{
			printf("select exit\n");
			Close();
			return false;
		}

		for (int i = (int)_clients.size() - 1; i >= 0; --i)
		{
			SOCKET clientSocket = _clients[i]->GetSocketfd();
			if (FD_ISSET(clientSocket, &fdRead))
			{
				if (-1 == RecvData(_clients[i]))
				{
					auto iter = _clients.begin() + i;
					if (iter != _clients.end())
					{
						if (_netEvent)
						{
							_netEvent->OnLeave(_clients[i]);
						}
						delete _clients[i];
						_clients.erase(iter);
					}
				}
			}
		}
	}

	return true;
}

int CellServer::RecvData(ClientSocket* client)
{
	// 5 接受客户端数据
	int lastPos = client->GetLastPos();
	char* szRecv = client->RecvBuf();
	char* msgBuf = client->MsgBuf();
	int recvLen = (int)recv(client->GetSocketfd(), szRecv, RECV_BUFF_SIZE, 0);
	if (recvLen <= 0)
	{
		// printf("client exit\n");
		return -1;
	}
	memcpy(msgBuf + lastPos, szRecv, recvLen);

	lastPos += recvLen;
	// 判断消息缓冲区数据长度是否大于消息头
	// 就可以知道当前消息的长度
	while (lastPos >= sizeof(DataHeader))
	{
		DataHeader* header = (DataHeader*)msgBuf;
		if (lastPos >= header->dataLength)
		{
			// 剩余消息缓冲区长度
			int size = lastPos - header->dataLength;
			OnNetMsg(client, header);
			if (size > 0)
			{
				memcpy(msgBuf, msgBuf + header->dataLength, size);
			}
			else
			{
				memcpy(msgBuf, msgBuf + header->dataLength, 1);
			}
			lastPos = size;
		}
		else
		{
			// 剩余数据不足一条完整消息
			break;
		}
	}

	client->SetLastPos(lastPos);
	return 0;
}

void CellServer::OnNetMsg(ClientSocket* client, DataHeader* header)
{
	// 6 处理请求
	// send 向客户端发送数据
	_netEvent->OnNetMsg(client, header);
}

void CellServer::Close()
{
	if (_sock == INVALID_SOCKET)
	{
		return;
	}

#ifdef _WIN32
	for (int i = (int)_clients.size() - 1; i >= 0; --i)
	{
		closesocket(_clients[i]->GetSocketfd());
		delete _clients[i];
	}
	// 关闭套接字closesocket;
	closesocket(_sock);
#else
	for (int i = (int)_clients.size() - 1; i >= 0; --i)
	{
		close(_clients[i]->GetSocketfd());
		delete _clients[i];
	}
	close(_sock);
#endif
	_clients.clear();
	_clientBuff.clear();
}

size_t CellServer::GetClientSize()
{
	return _clients.size() + _clientBuff.size();
}

void CellServer::SetEventObj(INetEvent* event)
{
	_netEvent = event;
}

void CellServer::AddClientBuff(ClientSocket* client)
{
	std::lock_guard<std::mutex> lg(_mutex);
	_clientBuff.push_back(client);
}

void CellServer::ClearClientBuff()
{
	std::lock_guard<std::mutex> lg(_mutex);
	_clientBuff.clear();
}

void CellServer::AddClientFromBuff()
{
	if (_clientBuff.empty())
	{
		return;
	}

	std::lock_guard<std::mutex> lg(_mutex);
	for (auto client : _clientBuff)
	{
		_clients.push_back(client);
	}
	_clientBuff.clear();
}
#pragma endregion

#pragma region EasyTcpServer
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
	SOCKET cSock = INVALID_SOCKET;
#ifdef _WIN32
	cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
#else
	cSock = accept(_sock, (sockaddr*)&clientAddr, (socklen_t*)&nAddrLen);
#endif // _WIN32

	if (INVALID_SOCKET == cSock)
	{
		printf("client socket invalid\n");
	}
	else
	{
		// printf("New Client : socket = %d, IP = %s \n", cSock, inet_ntoa(clientAddr.sin_addr));
		AddCellServer(new ClientSocket(cSock));
	}

	return _sock;
}

bool EasyTcpServer::OnRun()
{
	if (!IsRun())
	{
		return false;
	}

	Time4Msg();
	// 伯克利 socket
	fd_set fdRead;

	FD_ZERO(&fdRead);
	FD_SET(_sock, &fdRead);
	SOCKET maxSock = _sock;

	timeval t = { 0, 10 };
	// 第一个参数nfds是一个整数值，指fd_set集合中所有描述符(socket)的范围(即最大socket+1)
	// windows中不需要
	int ret = select(maxSock + 1, &fdRead, NULL, NULL, &t);
	if (ret < 0)
	{
		printf("server Accept select exit\n");
		Close();
		return false;
	}

	if (FD_ISSET(_sock, &fdRead))
	{
		FD_CLR(_sock, &fdRead);
		Accept();
		return true;
	}

	return true;
}

void EasyTcpServer::Close()
{
	if (_sock == INVALID_SOCKET)
	{
		return;
	}

#ifdef _WIN32
	// 关闭套接字closesocket;
	closesocket(_sock);
	WSACleanup(); // 和WSAStartup匹配
#else
	close(_sock);
#endif
}

int EasyTcpServer::SendData(SOCKET sock, const char* data, int length)
{
	if (IsRun() && data != NULL)
	{
		return send(sock, data, length, 0);
	}

	return SOCKET_ERROR;
}

void EasyTcpServer::Time4Msg()
{
	auto t1 = _tTime.GetElapsedSecond();
	if (t1 >= 1.0)
	{
		printf("socket : %d , _recvCount : %d , time : %lf \n", _sock, (int)_msgCount, ((int)_msgCount / t1));
		_msgCount = 0;
		_tTime.Update();
	}
}

void EasyTcpServer::Start(const int threadCpunt)
{
	for (int i = 0; i < threadCpunt; ++i)
	{
		auto server = new CellServer(_sock);
		_cellServers.push_back(server);
		// 注册事件
		server->SetEventObj(this);
		// 启动消息处理线程
		server->Start();
	}
}

void EasyTcpServer::AddCellServer(ClientSocket* client)
{
	auto server = GetMinClientCellServer();
	if (server == NULL)
	{
		return;
	}

	server->AddClientBuff(client);
	OnJoin(client);
}

CellServer* EasyTcpServer::GetMinClientCellServer()
{
	if (_cellServers.size() <= 0)
	{
		return NULL;
	}

	auto ret = _cellServers[0];
	for (auto& server : _cellServers)
	{
		if (server->GetClientSize() > ret->GetClientSize())
		{
			ret = server;
		}
	}

	return ret;
}

void EasyTcpServer::OnJoin(ClientSocket* client)
{
	++_clientCount;
}


void EasyTcpServer::OnLeave(ClientSocket* client)
{
	--_clientCount;
}

void EasyTcpServer::OnNetMsg(ClientSocket* client, DataHeader* header)
{
	++_msgCount;
}
#pragma endregion

