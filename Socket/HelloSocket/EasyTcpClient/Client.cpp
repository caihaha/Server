#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
// ���Ŀ¼ $(SolutionDir)../bin/$(Platform)/$(Configuration)
// �м�Ŀ¼ $(SolutionDir)../bin/$(Platform)/$(Configuration)/$(ProjectName)
int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data); // ����socket
	// TODO ��д�����̴���
	// 1 ����һ��Socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	// 2 ���ӷ���� connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(_sock, (sockaddr *)&_sin, sizeof(_sin)) == SOCKET_ERROR)
	{

	}

	WSACleanup(); // ��WSAStartupƥ��
	return 0;
}