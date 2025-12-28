#pragma once
#include <string>

// 观察者模式：消息接收者
class IMessageReceiver {
public:
    virtual ~IMessageReceiver() = default;
    virtual void OnMessageArrived(const std::string& msg) = 0;
};
