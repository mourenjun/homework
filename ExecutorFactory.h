#pragma once
#include "ITaskExecutor.h"
#include <memory>
#include <string>

// 工厂模式：任务创建器
class ExecutorFactory {
public:
    static std::shared_ptr<ITaskExecutor> Build(const std::string& type);
};
