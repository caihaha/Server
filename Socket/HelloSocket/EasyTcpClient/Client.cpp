#include "EasyTcpClient.h"
#include "EasyTcpClient.cpp"
#pragma once

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

const int cCount = 8;
const int tCount = 4;
const int count = cCount / tCount;
EasyTcpClient* client[cCount];

void SendThread(const int id)
{
	printf("thread <%d> start\n", id);

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
	}

	printf("thread <%d> , begin : %d, end : %d\n",id, begin, end);

	std::chrono::milliseconds t(3000);
	std::this_thread::sleep_for(t);

	Login login[10];
	for (int i = 0; i < 10; ++i)
	{
		strcpy_s(login[i].userName, "CJC");
		strcpy_s(login[i].PassWord, "123456");
	}

	const int length = sizeof(login);
	while (g_bRun)
	{
		for (int i = begin; i < end; ++i)
		{
			client[i]->SendData((const char*)(&login), length);
			/*if (!client[i]->OnRun())
			{
				continue;
			}*/
		}
	}

	for (int i = begin; i < end; ++i)
	{
		client[i]->Close();
		delete client[i];
	}
	printf("thread <%d> exit\n", id);
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