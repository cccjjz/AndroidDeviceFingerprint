#ifndef LOGGER_H
#define LOGGER_H

#include <android/log.h>
#include <string>

class Logger {
public:
    static void info(const std::string& tag, const std::string& message);
    static void error(const std::string& tag, const std::string& message);
    static void debug(const std::string& tag, const std::string& message);
    static void warn(const std::string& tag, const std::string& message);
    
    // 格式化日志方法
    template<typename... Args>
    static void info(const std::string& tag, const std::string& format, Args... args);
    
    template<typename... Args>
    static void error(const std::string& tag, const std::string& format, Args... args);
    
    template<typename... Args>
    static void debug(const std::string& tag, const std::string& format, Args... args);
    
    template<typename... Args>
    static void warn(const std::string& tag, const std::string& format, Args... args);

private:
    static std::string formatString(const std::string& format, ...);
};

// 便捷宏定义
#define LOGI(tag, ...) Logger::info(tag, __VA_ARGS__)
#define LOGE(tag, ...) Logger::error(tag, __VA_ARGS__)
#define LOGD(tag, ...) Logger::debug(tag, __VA_ARGS__)
#define LOGW(tag, ...) Logger::warn(tag, __VA_ARGS__)

#endif // LOGGER_H
