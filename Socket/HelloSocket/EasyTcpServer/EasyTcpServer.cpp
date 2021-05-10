#include "EasyTcpServer.h"

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
		AddCellServer(new CellClient(cSock));
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

void EasyTcpServer::AddCellServer(CellClient* client)
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

void EasyTcpServer::OnJoin(CellClient* client)
{
	++_clientCount;
}


void EasyTcpServer::OnLeave(CellClient* client)
{
	--_clientCount;
}

void EasyTcpServer::OnNetMsg(CellClient* client, DataHeader* header)
{
	++_msgCount;
}
#pragma endregion


