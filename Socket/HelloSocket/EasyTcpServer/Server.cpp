#include "EasyTcpServer.h"
#include "EasyTcpServer.cpp"
#pragma once

bool g_bRun = true;

void CmdThread()
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf_s("%s", cmdBuf, 256);
		if (0 == strcmp("exit", cmdBuf))
		{
			g_bRun = false;
			printf("thread exit\n");
			return;
		}
		else
		{
			printf("no this cmd\n");
		}
	}
}

class MyServer : public EasyTcpServer
{
public:
	MyServer() : EasyTcpServer()
	{
	}
	~MyServer()
	{
	}

	void OnJoin(ClientSocket* client);

	void OnLeave(ClientSocket* client);

	void OnNetMsg(ClientSocket* client, DataHeader* header);

private:

};

void MyServer::OnJoin(ClientSocket* client)
{
	++_clientCount;
	printf("client join, socket %d\n", client->GetSocketfd());
}


void MyServer::OnLeave(ClientSocket* client)
{
	--_clientCount;
	printf("client leave, socket %d\n", client->GetSocketfd());
}

void MyServer::OnNetMsg(ClientSocket* client, DataHeader* header)
{
	SOCKET sock = client->GetSocketfd();
	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		LoginResult inRet;
		client->SendData((const char*)&inRet, inRet.dataLength);
		break;
	}
	case CMD_LOGOUT:
	{
		LogoutResult outRet;
		client->SendData((const char*)&outRet, outRet.dataLength);
		break;
	}
	default:
	{
		DataHeader error = { 0, CMD_ERROR };
		client->SendData((const char*)&error, error.dataLength);
		printf("error cmd.\n");
	}
	}

	++_msgCount;
}

int main()
{
	MyServer server;
	server.InitSocket();
	server.Bind(NULL, 4567);
	server.Listen(5);
	server.Start(4);

	std::thread t1(CmdThread);
	t1.detach();

	while (g_bRun)
	{
		server.OnRun();
	}

	printf("server exit");
	getchar();
	return 0;
}