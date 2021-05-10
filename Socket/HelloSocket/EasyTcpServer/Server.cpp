#pragma once

#include "EasyTcpServer.h"
#include "EasyTcpServer.cpp"

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

	void OnJoin(CellClient* client);

	void OnLeave(CellClient* client);

	void OnNetMsg(CellClient* client, DataHeader* header, CellServer* cellServer);

private:

};

void MyServer::OnJoin(CellClient* client)
{
	EasyTcpServer::OnJoin(client);
	printf("client join, socket %d\n", client->GetSocketfd());
}


void MyServer::OnLeave(CellClient* client)
{
	EasyTcpServer::OnLeave(client);
	printf("client leave, socket %d\n", client->GetSocketfd());
}

void MyServer::OnNetMsg(CellClient* client, DataHeader* header, CellServer* cellServer)
{
	EasyTcpServer::OnNetMsg(client, header, cellServer);

	SOCKET sock = client->GetSocketfd();
	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		LoginResult inRet;
		client->SendData((const char*)&inRet, inRet.dataLength);

		//LoginResult *inRet = new LoginResult();
		//cellServer->AddSendTask(client, inRet);
		break;
	}
	case CMD_LOGOUT:
	{
		LogoutResult outRet;
		client->SendData((const char*)&outRet, outRet.dataLength);
		break;
	}
	case CMD_HEART:
	{
		client->ResetDTHeart();
		break;
	}
	default:
	{
		DataHeader error = { CMD_ERROR, sizeof(DataHeader) };
		client->SendData((const char*)&error, error.dataLength);
		printf("error cmd.\n");
	}
	}
}

int main()
{
	MyServer server;
	server.InitSocket();
	server.Bind(NULL, 4567);
	server.Listen(5);
	server.Start(1);

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