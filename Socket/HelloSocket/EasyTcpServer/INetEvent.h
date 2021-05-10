#pragma once
#ifndef _I_NET_EVENT_H_
#define _I_NET_EVENT_H_

#include "CELLClient.h"
#include "CELLServer.h"

class CellServer;
#pragma region INetEvent
// �¼�����
class INetEvent
{
public:
	INetEvent()
	{
	}
	virtual ~INetEvent()
	{
	}

	// �ͻ��˼����¼�
	virtual void OnJoin(CellClient* client) = 0;
	// �ͻ����뿪�¼�
	virtual void OnLeave(CellClient* client) = 0;
	// �ͻ�����Ϣʱ��
	virtual void OnNetMsg(CellClient* client, DataHeader* header, CellServer* cellServer) = 0;

private:

};
#pragma endregion

#endif // !_I_NET_EVENT_H_
