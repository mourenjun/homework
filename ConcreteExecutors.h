#pragma once
#include "ITaskExecutor.h"
#include "TaskEngine.h"
#include <string>
#include <thread>
#include <mutex>
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>

static std::mutex g_sharedResource_A;
static std::mutex g_sharedResource_B;

// ========== 基础任务 ==========

class MatrixExecutor : public ITaskExecutor {
public:
    std::string GetID() const override { return "Matrix Calc"; }
    void Run() override {
        TaskEngine::Instance()->GetLogger().Log("[Matrix] 矩阵运算...");
        std::this_thread::sleep_for(std::chrono::milliseconds(600));

        std::ostringstream oss;
        oss << "结果矩阵[200x200] 耗时: 0.6s";
        TaskEngine::Instance()->Broadcast("[DATA-MATRIX] " + oss.str());
        TaskEngine::Instance()->GetLogger().Log("[Matrix] 计算完成");
    }
};

class BackupExecutor : public ITaskExecutor {
public:
    std::string GetID() const override { return "Data Backup"; }
    void Run() override {
        auto& log = TaskEngine::Instance()->GetLogger();
        log.Log("[Backup] 启动PowerShell压缩流程...");

        std::string cmd = "powershell -NoProfile -Command \"Compress-Archive -Path C:\\Data\\* -DestinationPath D:\\backup.zip -Force\"";
        system(cmd.c_str());

        log.Log("[Backup] 备份完成 → D:\\backup.zip");
    }
};

class WebExecutor : public ITaskExecutor {
public:
    std::string GetID() const override { return "Web Request"; }
    void Run() override {
        TaskEngine::Instance()->GetLogger().Log("[Web] 发起HTTP请求...");
        std::this_thread::sleep_for(std::chrono::milliseconds(400));

        TaskEngine::Instance()->Broadcast("[DATA-WEB] 状态码: 200 OK");
        TaskEngine::Instance()->GetLogger().Log("[Web] 请求成功");
    }
};

class AlertExecutor : public ITaskExecutor {
public:
    std::string GetID() const override { return "Alert Service"; }
    void Run() override {
        TaskEngine::Instance()->GetLogger().Log("[Alert] 检查提醒事项...");
        ::MessageBox(NULL, _T("该完成今日任务了！"), _T("提醒服务"), MB_OK | MB_ICONINFORMATION);
        TaskEngine::Instance()->GetLogger().Log("[Alert] 提醒已发送");
    }
};

class AnalyticsExecutor : public ITaskExecutor {
public:
    std::string GetID() const override { return "Data Analytics"; }
    void Run() override {
        TaskEngine::Instance()->GetLogger().Log("[Analytics] 正在分析数据集...");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 100);

        double sum = 0;
        for (int i = 0; i < 1000; ++i) sum += dis(gen);
        double avg = sum / 1000.0;

        std::ostringstream oss;
        oss << "均值: " << std::fixed << std::setprecision(2) << avg;
        TaskEngine::Instance()->Broadcast("[DATA-STATS] " + oss.str());
    }
};

// ========== 死锁演示 ==========

class DeadlockDemoExecutor : public ITaskExecutor {
public:
    std::string GetID() const override { return "Deadlock Demo"; }
    void Run() override {
        auto& log = TaskEngine::Instance()->GetLogger();

        log.Log("线程A: 准备获取资源A...");
        g_sharedResource_A.lock();
        TaskEngine::Instance()->Broadcast("线程A: 已锁定资源A");

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        log.Log("线程A: 准备获取资源B...");
        // 此时如果线程B也在等资源A，就会死锁
        g_sharedResource_B.lock();
        TaskEngine::Instance()->Broadcast("线程A: 已锁定资源B");

        // 模拟异常崩溃
        log.Log("线程A: 发生错误！");
        throw std::runtime_error("Simulated Crash in Thread A");

        // 永远执行不到
        g_sharedResource_B.unlock();
        g_sharedResource_A.unlock();
    }
};

class SafeDemoExecutor : public ITaskExecutor {
public:
    std::string GetID() const override { return "Safe Demo"; }
    void Run() override {
        auto& log = TaskEngine::Instance()->GetLogger();

        log.Log("[Safe] 线程B: 获取资源A...");
        std::unique_lock<std::mutex> lockA(g_sharedResource_A);
        TaskEngine::Instance()->Broadcast("[Safe] 线程B: 已安全锁定资源A");

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        log.Log("[Safe] 线程B: 获取资源B...");
        std::unique_lock<std::mutex> lockB(g_sharedResource_B);
        TaskEngine::Instance()->Broadcast("[Safe] 线程B: 已安全锁定资源B");

        log.Log("[Safe] 线程B: 模拟异常...");
        throw std::runtime_error("Simulated Crash with Protection");

        // 自动释放锁
    }
};

class ValidatorExecutor : public ITaskExecutor {
public:
    std::string GetID() const override { return "Validator"; }
    void Run() override {
        auto& log = TaskEngine::Instance()->GetLogger();

        log.Log("[Validator] 验证线程: 尝试获取资源A...");
        TaskEngine::Instance()->Broadcast("[Validator] 等待资源A释放...");

        std::lock_guard<std::mutex> lock(g_sharedResource_A);

        log.Log("[Validator] 锁被正确释放！获取了资源A！");
        TaskEngine::Instance()->Broadcast("[Validator] 验证通过：防死锁机制有效！");
    }
};
