#include "EasyTcpServer.hpp"

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

int main()
{
	EasyTcpServer server;
	server.InitSocket();
	server.Bind(NULL, 4567);
	server.Listen(5);
	server.Start();

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