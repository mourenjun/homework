#pragma once
#include "TaskWrapper.h"
#include "FileLogger.h"
#include "IMessageReceiver.h"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>

// 单例模式：任务引擎
class TaskEngine {
private:
    static TaskEngine* instance;

    std::priority_queue<TaskWrapper, std::vector<TaskWrapper>, std::greater<TaskWrapper>> taskQueue;
    std::vector<IMessageReceiver*> receivers;
    std::mutex receiverMutex;
    std::mutex queueMutex;
    std::condition_variable cvQueue;
    std::atomic<bool> engineStopped;
    std::thread engineThread;
    FileLogger logger;

    TaskEngine();
    void EngineLoop();

public:
    TaskEngine(const TaskEngine&) = delete;
    TaskEngine& operator=(const TaskEngine&) = delete;

    static TaskEngine* Instance();

    void Subscribe(IMessageReceiver* receiver);
    void Unsubscribe(IMessageReceiver* receiver);
    void Broadcast(const std::string& msg);

    FileLogger& GetLogger() { return logger; }

    void SubmitTask(std::shared_ptr<ITaskExecutor> executor, int delayMs, bool repeat = false, int intervalMs = 0);

    void StartEngine();
    void StopEngine();
};
