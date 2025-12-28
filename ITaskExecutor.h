#pragma once
#include <string>

// 策略模式：任务执行器接口
class ITaskExecutor {
public:
    virtual ~ITaskExecutor() = default;
    virtual void Run() = 0;
    virtual std::string GetID() const = 0;
};
