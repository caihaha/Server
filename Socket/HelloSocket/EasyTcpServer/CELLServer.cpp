#include "CELLServer.h"

#pragma region CellServer
void CellServer::Start()
{
	_thread = std::thread(std::mem_fn(&CellServer::OnRun), this);
	_taskServer.Start();
	_oldTime = CELLTime::GetNowInMilliSec();
}

bool CellServer::OnRun()
{
	_isFdReadChange = true;
	while (IsRun())
	{
		AddClientFromBuff();

		if (_sock2Clients.empty())
		{
			std::chrono::milliseconds t(1);
			std::this_thread::sleep_for(t);
			continue;
		}

		// ������ socket
		fd_set fdRead;
		FD_ZERO(&fdRead);

		SOCKET maxSock = _sock;
		if (_isFdReadChange)
		{
			for (auto& iter : _sock2Clients)
			{
				SOCKET clientSocket = iter.first;
				FD_SET(clientSocket, &fdRead);
				if (clientSocket > maxSock)
				{
					maxSock = clientSocket;
				}
			}
			memcpy_s(&_clientFdRead, (size_t)sizeof(_clientFdRead), &fdRead, (size_t)sizeof(fd_set));
			_isFdReadChange = false;
		}
		else
		{
			memcpy_s(&fdRead, (size_t)sizeof(fd_set), &_clientFdRead, (size_t)sizeof(_clientFdRead));
		}

		timeval t = { 0, 1 };
		// ��һ������nfds��һ������ֵ��ָfd_set����������������(socket)�ķ�Χ(�����socket+1)
		// windows�в���Ҫ
		int ret = select(maxSock + 1, &fdRead, NULL, NULL, &t);
		if (ret < 0)
		{
			printf("select exit\n");
			Close();
			return false;
		}

		ReadData(fdRead);
	}
	return true;
}

void CellServer::ReadData(fd_set fdRead)
{
#ifdef _WIN32
	for (int i = 0; i < fdRead.fd_count; ++i)
	{
		auto iter = _sock2Clients.find(fdRead.fd_array[i]);
		if (iter != _sock2Clients.end())
		{
			if (-1 == RecvData(iter->second))
			{
				_isFdReadChange = true;

				if (_netEvent)
				{
					_netEvent->OnLeave(iter->second);
				}
				delete iter->second;
				_sock2Clients.erase(iter);
			}
		}
	}
#else
	for (auto iter = _sock2Clients.begin(); iter != _sock2Clients.end(); ++iter)
	{
		if (FD_ISSET(iter->first, &fdRead))
		{
			if (-1 == RecvData(iter->second))
			{
				_isFdReadChange = true;

				if (_netEvent)
				{
					_netEvent->OnLeave(iter->second);
				}
				delete iter->second;
				_sock2Clients.erase(iter++);
			}
			else
			{
				++iter;
			}
		}
	}
#endif // _WIN32
}

void CellServer::CheckTime()
{
	time_t nowTime = CELLTime::GetNowInMilliSec();
	time_t deltaTime = nowTime - _oldTime;
	_oldTime = nowTime;
	for (auto iter = _sock2Clients.begin(); iter != _sock2Clients.end();)
	{
		if (iter->second->CheckHeart(deltaTime))
		{
			_isFdReadChange = true;

			if (_netEvent)
			{
				_netEvent->OnLeave(iter->second);
			}
			delete iter->second;
			_sock2Clients.erase(iter++);
		}
		else
		{
			++iter;
		}
	}
}

int CellServer::RecvData(CellClient* client)
{
	// 5 ���ܿͻ�������
	int lastPos = client->GetLastPos();
	char* szRecv = client->RecvBuf();
	char* msgBuf = client->MsgBuf();
	int recvLen = (int)recv(client->GetSocketfd(), szRecv, RECV_BUFF_SIZE, 0);
	if (recvLen <= 0)
	{
		// printf("client exit\n");
		return -1;
	}
	memcpy(msgBuf + lastPos, szRecv, recvLen);

	lastPos += recvLen;
	// �ж���Ϣ���������ݳ����Ƿ������Ϣͷ
	// �Ϳ���֪����ǰ��Ϣ�ĳ���
	while (lastPos >= sizeof(DataHeader))
	{
		DataHeader* header = (DataHeader*)msgBuf;
		if (lastPos >= header->dataLength)
		{
			// ʣ����Ϣ����������
			int size = lastPos - header->dataLength;
			OnNetMsg(client, header);
			memcpy(msgBuf, msgBuf + header->dataLength, size);

			lastPos = size;
		}
		else
		{
			// ʣ�����ݲ���һ��������Ϣ
			break;
		}
	}

	client->SetLastPos(lastPos);
	return 0;
}

void CellServer::OnNetMsg(CellClient* client, DataHeader* header)
{
	// 6 ��������
	// send ��ͻ��˷�������
	_netEvent->OnNetMsg(client, header, this);
}

void CellServer::Close()
{
	if (_sock == INVALID_SOCKET)
	{
		return;
	}

#ifdef _WIN32
	for (auto iter = _sock2Clients.rbegin(); iter != _sock2Clients.rend(); ++iter)
	{
		delete iter->second;
	}
	// �ر��׽���closesocket;
	closesocket(_sock);
#else
	for (auto iter = _sock2Clients.rbegin(); iter != _sock2Clients.rend(); ++iter)
	{
		delete iter->second;
	}
	close(_sock);
#endif
	_sock2Clients.clear();
	_clientBuff.clear();

	_taskServer.Close();
}

size_t CellServer::GetClientSize()
{
	return _sock2Clients.size() + _clientBuff.size();
}

void CellServer::SetEventObj(INetEvent* event)
{
	_netEvent = event;
}

void CellServer::AddClientBuff(CellClient* client)
{
	std::lock_guard<std::mutex> lg(_mutex);
	_clientBuff.push_back(client);
}

void CellServer::ClearClientBuff()
{
	std::lock_guard<std::mutex> lg(_mutex);
	_clientBuff.clear();
}

void CellServer::AddClientFromBuff()
{
	if (_clientBuff.empty())
	{
		return;
	}

	std::lock_guard<std::mutex> lg(_mutex);
	for (auto client : _clientBuff)
	{
		_sock2Clients[client->GetSocketfd()] = client;
	}
	_clientBuff.clear();
	_isFdReadChange = true;
}

void CellServer::AddSendTask(CellClient* client, DataHeader *header)
{
	if (header == nullptr)
	{
		return;
	}

	_taskServer.AddTaskToBuf([client, header]() {
		switch (header->cmd)
		{
		case CMD_LOGIN:
			client->SendData((const char*)((Login*)header), header->dataLength);
			break;
		case CMD_LOGOUT:
			client->SendData((const char*)((Logout*)header), header->dataLength);
			break;
		default:
			break;
		}

		delete header; 
		});
}

#pragma endregion