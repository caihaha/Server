#ifndef _CELL_THREAD_H_
#define _CELL_THREAD_H_

#include <functional>

#include "CELLSemaphore.h"

class CELLThread
{
private:
	typedef std::function<void(CELLThread*)> EventCall;

public:
	CELLThread()
	{
		_onCreate = NULL;
		_onWorkCall = NULL;
		_onDestory = NULL;
	}
	~CELLThread()
	{
		_onCreate = NULL;
		_onWorkCall = NULL;
		_onDestory = NULL;
	}

	void Start(EventCall create = NULL, EventCall work = NULL, EventCall destory = NULL)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		if (_isRun)
		{
			return;
		}
		
		_isRun = true;
		_onCreate = create;
		_onWorkCall = work;
		_onDestory = destory;

		std::thread t(std::mem_fn(&CELLThread::OnWork), this);
		t.detach();
	}

	void Close()
	{
		std::lock_guard<std::mutex> lock(_mutex);

		if (!_isRun)
		{
			return;
		}

		_isRun = false;
		_sem.wait();
	}

	void Exit()
	{
		std::lock_guard<std::mutex> lock(_mutex);

		if (!_isRun)
		{
			return;
		}

		_isRun = false;
	}

	bool IsRun()
	{
		return _isRun;
	}
private:
	void OnWork()
	{
		if (_onCreate)
		{
			_onCreate(this);
		}
		if (_onWorkCall)
		{
			_onWorkCall(this);
		}
		if (_onDestory)
		{
			_onDestory(this);
		}

		_sem.wakeup();
	}

private:

	EventCall _onCreate;
	EventCall _onWorkCall;
	EventCall _onDestory;

	bool _isRun;

	CELLSemaphore _sem;

	std::mutex _mutex;
};

#endif // !_CELL_THREAD_H_
