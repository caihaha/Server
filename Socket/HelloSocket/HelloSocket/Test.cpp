#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <windows.h>

// #pragma comment(lib, "ws2_32.lib")

int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data); // ����socket
	// TODO ��д�����̴���

	WSACleanup(); // ��WSAStartupƥ��
	return 0;
}