#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
// 输出目录 $(SolutionDir)../bin/$(Platform)/$(Configuration)
// 中间目录 $(SolutionDir)../bin/$(Platform)/$(Configuration)/$(ProjectName)
int main()
{
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data); // 启动socket
	// TODO 编写网络编程代码
	// 1 建立一个Socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	// 2 连接服务端 connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(_sock, (sockaddr *)&_sin, sizeof(_sin)) == SOCKET_ERROR)
	{

	}

	WSACleanup(); // 和WSAStartup匹配
	return 0;
}