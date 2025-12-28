#include "pch.h"
#include "ExecutorFactory.h"
#include "ConcreteExecutors.h"

std::shared_ptr<ITaskExecutor> ExecutorFactory::Build(const std::string& type) {
    if (type == "Matrix") return std::make_shared<MatrixExecutor>();
    if (type == "Backup") return std::make_shared<BackupExecutor>();
    if (type == "WebRequest") return std::make_shared<WebExecutor>();
    if (type == "Alert") return std::make_shared<AlertExecutor>();
    if (type == "Analytics") return std::make_shared<AnalyticsExecutor>();
    if (type == "DeadlockDemo") return std::make_shared<DeadlockDemoExecutor>();
    if (type == "SafeDemo") return std::make_shared<SafeDemoExecutor>();
    if (type == "Validator") return std::make_shared<ValidatorExecutor>();
    return nullptr;
}

