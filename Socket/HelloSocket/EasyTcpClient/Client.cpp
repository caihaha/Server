#include "EasyTcpClient.hpp"

void CmdThread(EasyTcpClient* client)
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf_s("%s", cmdBuf, 256);
		if (0 == strcmp("exit", cmdBuf))
		{
			client->Close();
			printf("thread exit\n");
			return;
		}
		else if (0 == strcmp("login", cmdBuf))
		{
			Login login;
			strcpy_s(login.userName, "CJC");
			strcpy_s(login.PassWord, "123456");
			client->SendData(&login);
		}
		else if (0 == strcmp("logout", cmdBuf))
		{
			Logout logout;
			strcpy_s(logout.userName, "CJC");
			client->SendData(&logout);
		}
		else
		{
			printf("no this cmd\n");
		}
	}
}

int main()
{
    EasyTcpClient client;
	client.InitSocket();
	client.Connect("127.0.0.1", 4567);
	std::thread t1(CmdThread, &client);
	t1.detach();

	while (client.IsRun())
	{
		client.OnRun();
	}

	client.Close();
	printf("client exit\n");
	getchar();
    return 0;
}