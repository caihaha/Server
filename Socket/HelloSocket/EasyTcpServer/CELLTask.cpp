#include "CELLTask.h"

#pragma region CellTaskServer
void CellTaskServer::Start()
{
    _thread.Start(  NULL,
                    [this](CELLThread* t) {OnRun(t); },
                    NULL);
}

void CellTaskServer::AddTaskToBuf(CellTask task)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _taskListBuf.push_back(task);
}

void CellTaskServer::OnRun(CELLThread* t)
{
    while (t->IsRun())
    {
        AddTaskFromBuf();

        if (_taskList.empty())
        {
            std::chrono::milliseconds t(1);
            std::this_thread::sleep_for(t);
            continue;
        }

        // 处理任务
        for (auto& task : _taskList)
        {
            task();
        }

        _taskList.clear();
    }
}

void CellTaskServer::Close()
{
    _thread.Close();
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