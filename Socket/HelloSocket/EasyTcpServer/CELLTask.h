#pragma once
#ifndef _CELL_TASK_H_
#define _CELL_TASK_H_
#include <thread>
#include <atomic>
#include <mutex>
#include <list>
#include <functional>

#include "CELLThread.h"

#pragma region 执行任务类
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
	// 任务数据
	std::list<CellTask> _taskList;
	// 任务数据缓冲区
	std::list<CellTask> _taskListBuf;
	// 数据缓冲区锁
	std::mutex _mutex;

	CELLThread _thread;
};
#pragma endregion

#endif // _CELL_TASK_H_
