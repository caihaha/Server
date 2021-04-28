#include "EasyTcpClient.h"
#include "EasyTcpClient.cpp"

int g_bRun = true;
void CmdThread()
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf_s("%s", cmdBuf, 256);
		if (0 == strcmp("exit", cmdBuf))
		{
			// client->Close();
			g_bRun = false;
			printf("thread exit\n");
			return;
		}
		else if (0 == strcmp("login", cmdBuf))
		{
			//Login login;
			//strcpy_s(login.userName, "CJC");
			//strcpy_s(login.PassWord, "123456");
			//client->SendData((const char *)&login, login.dataLength);
		}
		else if (0 == strcmp("logout", cmdBuf))
		{
			//Logout logout;
			//strcpy_s(logout.userName, "CJC");
			//client->SendData((const char*)&logout, logout.dataLength);
		}
		else
		{
			printf("no this cmd\n");
		}
	}
}

const int cCount = 1000;
const int tCount = 4;
const int count = cCount / tCount;
EasyTcpClient* client[cCount];

void SendThread(int id)
{
	if (!g_bRun)
	{
		return;
	}

	int begin = (id - 1) * count;
	int end = id * count;

	for (int i = begin; i < end; ++i)
	{
		client[i] = new EasyTcpClient();
	}

	for (int i = begin; i < end; ++i)
	{
		client[i]->InitSocket();
		client[i]->Connect("127.0.0.1", 4567);
		printf("connect count : %d", i);
	}

	Login login;
	strcpy_s(login.userName, "CJC");
	strcpy_s(login.PassWord, "123456");

	while (g_bRun)
	{
		for (int i = begin; i < end; ++i)
		{
			client[i]->SendData((const char*)(&login), login.dataLength);
			/*if (!client[i]->OnRun())
			{
				continue;
			}*/
		}
	}

	for (int i = begin; i < end; ++i)
	{
		client[i]->Close();
	}
}

int main()
{
	if (!g_bRun)
	{
		return 0;
	}

	std::thread t1(CmdThread);
	t1.detach();

	for (int i = 0; i < tCount; ++i)
	{
		std::thread t(SendThread, i + 1);
		t.detach();
	}

	printf("all client exit\n");
	getchar();
    return 0;
}