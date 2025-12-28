#pragma once
#include <fstream>
#include <string>
#include <mutex>

// RAIIÈÕÖ¾¼ÇÂ¼Æ÷
class FileLogger {
private:
    std::ofstream file;
    std::mutex mtx;

public:
    explicit FileLogger(const std::string& path) {
        file.open(path, std::ios::app);
    }

    ~FileLogger() {
        if (file.is_open()) file.close();
    }

    void Log(const std::string& text) {
        std::lock_guard<std::mutex> lock(mtx);
        if (file.is_open()) {
            file << text << std::endl;
        }
    }
};
