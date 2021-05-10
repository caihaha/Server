#include "CELLTask.h"

#pragma region CellTaskServer
void CellTaskServer::Start()
{
    std::thread t(std::mem_fn(&CellTaskServer::OnRun), this);
    t.detach();
}

void CellTaskServer::AddTaskToBuf(CellTask task)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _taskListBuf.push_back(task);
}

void CellTaskServer::OnRun()
{
    while (true)
    {
        AddTaskFromBuf();

        if (_taskList.empty())
        {
            std::chrono::milliseconds t(1);
            std::this_thread::sleep_for(t);
            continue;
        }

        // ��������
        for (auto& task : _taskList)
        {
            task();
        }

        _taskList.clear();
    }
}

void CellTaskServer::Close()
{

}

void CellTaskServer::AddTaskFromBuf()
{
    if (_taskListBuf.empty())
    {
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& task : _taskListBuf)
    {
        _taskList.push_back(task);
    }
    _taskListBuf.clear();
}
#pragma endregion