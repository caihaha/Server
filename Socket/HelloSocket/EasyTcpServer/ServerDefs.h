#pragma once
#ifndef _SERVER_DEFS_H_
#define _SERVER_DEFS_H_

#ifdef _WIN32
#define FD_SETSIZE 1024
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd>
#include <arpa/inet.h>
#include <string.h>
#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(0)
#define SOCKET_ERROR (-1)
#endif

#include "DataDef.hpp"
#include "CELLTimestamp.hpp"
#include "CELLTask.h"

#ifndef RECV_BUFF_SIZE
#define RECV_BUFF_SIZE 10240
#endif // !RECV_BUFF_SIZE

#ifndef SEND_BUFF_SIZE
#define SEND_BUFF_SIZE 1024
#endif // !SEND_BUFF_SIZE

// ¿Í»§¶ËÐÄÌø¼ì²â(ºÁÃë)
#ifndef CLIENT_HEART_DEAD_TIME
#define CLIENT_HEART_DEAD_TIME 5000
#endif // !CLIENT_HEART_DEAD_TIME

#endif // !_SERVER_DEFS_H_
