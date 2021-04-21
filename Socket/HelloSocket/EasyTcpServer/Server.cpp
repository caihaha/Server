#include "EasyTcpServer.hpp"


int main()
{
	EasyTcpServer server;
	server.InitSocket();
	server.Bind(NULL, 4567);
	server.Listen(5);

	while (server.IsRun())
	{
		server.OnRun();
	}

	printf("server exit");
	getchar();
	return 0;
}