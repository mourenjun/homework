#pragma once
#include "ITaskExecutor.h"
#include <memory>
#include <chrono>

// 命令模式：任务包装器
struct TaskWrapper {
    std::shared_ptr<ITaskExecutor> executor;
    std::chrono::system_clock::time_point triggerTime;
    bool isRepeating;
    std::chrono::milliseconds repeatInterval;

    TaskWrapper(std::shared_ptr<ITaskExecutor> exec,
        std::chrono::system_clock::time_point time,
        bool repeat = false,
        int intervalMs = 0)
        : executor(exec), triggerTime(time),
        isRepeating(repeat), repeatInterval(intervalMs) {
    }

    bool operator>(const TaskWrapper& other) const {
        return triggerTime > other.triggerTime;
    }
};
