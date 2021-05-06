#pragma once
#ifndef _CELL_TASK_H_
#define _CELL_TASK_H_
#include <thread>
#include <atomic>
#include <mutex>
#include <list>
#include <functional>

#pragma region �������
class CellTask
{
public:
	CellTask()
	{
	}
	virtual ~CellTask()
	{
	}

	virtual void DoTask()
	{

	}
private:

};

#pragma endregion

#pragma region ִ��������
class CellTaskServer
{
public:
	CellTaskServer()
	{
	}
	virtual ~CellTaskServer()
	{
	}

	void Start();

	void AddTaskToBuf(CellTask* task);

	void Stop();

private:
	void OnRun();

	void AddTaskFromBuf();

private:
	// ��������
	std::list<CellTask*> _taskList;
	// �������ݻ�����
	std::list<CellTask*> _taskListBuf;
	// ���ݻ�������
	std::mutex _mutex;

};
#pragma endregion

#endif // _CELL_TASK_H_
