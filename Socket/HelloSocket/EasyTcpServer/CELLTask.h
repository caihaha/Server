#pragma once
#ifndef _CELL_TASK_H_
#define _CELL_TASK_H_
#include <thread>
#include <atomic>
#include <mutex>
#include <list>
#include <functional>

#include "CELLThread.h"

#pragma region ִ��������
class CellTaskServer
{
	typedef std::function<void()> CellTask;
public:
	CellTaskServer()
	{
	}
	virtual ~CellTaskServer()
	{
	}

	void Start();

	void AddTaskToBuf(CellTask task);

	void Close();

private:
	void OnRun(CELLThread* t);

	void AddTaskFromBuf();

private:
	// ��������
	std::list<CellTask> _taskList;
	// �������ݻ�����
	std::list<CellTask> _taskListBuf;
	// ���ݻ�������
	std::mutex _mutex;

	CELLThread _thread;
};
#pragma endregion

#endif // _CELL_TASK_H_
