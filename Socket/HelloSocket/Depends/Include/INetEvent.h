#pragma once
#ifndef _I_NET_EVENT_H_
#define _I_NET_EVENT_H_

#include "CELLClient.h"
#include "CELLServer.h"

class CellServer;
#pragma region INetEvent
// 事件处理
class INetEvent
{
public:
	INetEvent()
	{
	}
	virtual ~INetEvent()
	{
	}

	// 客户端加入事件
	virtual void OnJoin(CellClient* client) = 0;
	// 客户端离开事件
	virtual void OnLeave(CellClient* client) = 0;
	// 客户端消息时间
	virtual void OnNetMsg(CellClient* client, DataHeader* header, CellServer* cellServer) = 0;

private:

};
#pragma endregion

#endif // !_I_NET_EVENT_H_
