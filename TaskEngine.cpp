#include "pch.h"
#include "TaskEngine.h"

TaskEngine* TaskEngine::instance = nullptr;

TaskEngine::TaskEngine() : logger("engine_log.txt"), engineStopped(false) {}

TaskEngine* TaskEngine::Instance() {
    if (instance == nullptr) {
        instance = new TaskEngine();
    }
    return instance;
}

void TaskEngine::Subscribe(IMessageReceiver* receiver) {
    std::lock_guard<std::mutex> lock(receiverMutex);
    receivers.push_back(receiver);
}

void TaskEngine::Unsubscribe(IMessageReceiver* receiver) {
    std::lock_guard<std::mutex> lock(receiverMutex);
    receivers.erase(std::remove(receivers.begin(), receivers.end(), receiver), receivers.end());
}

void TaskEngine::Broadcast(const std::string& msg) {
    std::lock_guard<std::mutex> lock(receiverMutex);
    for (auto r : receivers) {
        if (r) r->OnMessageArrived(msg);
    }
}

void TaskEngine::StartEngine() {
    if (engineStopped) engineStopped = false;

    if (!engineThread.joinable()) {
        engineThread = std::thread(&TaskEngine::EngineLoop, this);
        logger.Log("[Engine] 引擎已启动");
    }
}

void TaskEngine::StopEngine() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        engineStopped = true;
    }
    cvQueue.notify_all();

    if (engineThread.joinable()) {
        engineThread.join();
    }
    logger.Log("[Engine] 引擎已停止");
}

void TaskEngine::SubmitTask(std::shared_ptr<ITaskExecutor> executor, int delayMs, bool repeat, int intervalMs) {
    auto now = std::chrono::system_clock::now();
    auto triggerTime = now + std::chrono::milliseconds(delayMs);

    TaskWrapper wrapper(executor, triggerTime, repeat, intervalMs);

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(wrapper);
        logger.Log("[Engine] 任务已提交: " + executor->GetID());
    }
    cvQueue.notify_one();
}

void TaskEngine::EngineLoop() {
    while (true) {
        std::shared_ptr<ITaskExecutor> executor = nullptr;
        bool repeat = false;
        int intervalMs = 0;

        {
            std::unique_lock<std::mutex> lock(queueMutex);

            cvQueue.wait(lock, [this] {
                return engineStopped || !taskQueue.empty();
                });

            if (engineStopped && taskQueue.empty()) break;

            const auto& top = taskQueue.top();
            auto now = std::chrono::system_clock::now();

            if (now >= top.triggerTime) {
                executor = top.executor;
                repeat = top.isRepeating;
                intervalMs = static_cast<int>(top.repeatInterval.count());
                taskQueue.pop();
            }
            else {
                cvQueue.wait_until(lock, top.triggerTime);
                continue;
            }
        }

        if (executor) {
            try {
                logger.Log("[Engine] 执行中: " + executor->GetID());
                Broadcast("[Running] " + executor->GetID());

                executor->Run();

                logger.Log("[Engine] 完成: " + executor->GetID());

                if (repeat) {
                    SubmitTask(executor, intervalMs, true, intervalMs);
                }
            }
            catch (const std::exception& e) {
                logger.Log("[Engine] 异常: " + executor->GetID() + " - " + e.what());
            }
        }
    }
}
