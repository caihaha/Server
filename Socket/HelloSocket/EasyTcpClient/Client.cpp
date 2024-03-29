#pragma once

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

const int cCount = 1;
const int tCount = 1;
const int count = cCount / tCount;
EasyTcpClient* client[cCount];
std::atomic_int readyCount = 0;

void RecvThread(int begin, int end)
{
	while (g_bRun)
	{
		for (int i = begin; i < end; ++i)
		{
			client[i]->OnRun();
		}
	}
}

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
	// 心跳检测：死亡计时，客户端每隔一段时间要向服务端发送一条消息
	// 
	// 
	printf("thread <%d> , begin : %d, end : %d\n",id, begin, end);

	// 等待所有线程都准备好
	++readyCount;
	while (readyCount < tCount)
	{
		std::chrono::milliseconds t(10);
		std::this_thread::sleep_for(t);
	}

	std::thread t1(RecvThread, begin, end);
	t1.detach();

	Login login[1];
	for (int i = 0; i < 1; ++i)
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
		}
		// std::chrono::milliseconds t(10);
		// std::this_thread::sleep_for(t);
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

	CELLTimestamp tTime;
	while (g_bRun)
	{
		auto t = tTime.GetElapsedSecond();
		if (t >= 1.0)
		{
			printf("thread<%d>, clients<%d>, time<%lf>\n", tCount, cCount, t);
			tTime.Update();
		}
		Sleep(1);
	}

	printf("all client exit\n");
	getchar();
    return 0;
}