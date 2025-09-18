#include "../include/Logger.h"
#include <cstdarg>
#include <cstdio>
#include <memory>

void Logger::info(const std::string& tag, const std::string& message) {
    __android_log_print(ANDROID_LOG_INFO, tag.c_str(), "%s", message.c_str());
}

void Logger::error(const std::string& tag, const std::string& message) {
    __android_log_print(ANDROID_LOG_ERROR, tag.c_str(), "%s", message.c_str());
}

void Logger::debug(const std::string& tag, const std::string& message) {
    __android_log_print(ANDROID_LOG_DEBUG, tag.c_str(), "%s", message.c_str());
}

void Logger::warn(const std::string& tag, const std::string& message) {
    __android_log_print(ANDROID_LOG_WARN, tag.c_str(), "%s", message.c_str());
}

std::string Logger::formatString(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    
    // 获取格式化字符串的长度
    va_list args_copy;
    va_copy(args_copy, args);
    int length = vsnprintf(nullptr, 0, format.c_str(), args_copy);
    va_end(args_copy);
    
    if (length <= 0) {
        va_end(args);
        return "";
    }
    
    // 创建缓冲区并格式化字符串
    std::string result(length, '\0');
    vsnprintf(&result[0], length + 1, format.c_str(), args);
    va_end(args);
    
    return result;
}

// 模板方法的实现
template<typename... Args>
void Logger::info(const std::string& tag, const std::string& format, Args... args) {
    std::string message = formatString(format, args...);
    info(tag, message);
}

template<typename... Args>
void Logger::error(const std::string& tag, const std::string& format, Args... args) {
    std::string message = formatString(format, args...);
    error(tag, message);
}

template<typename... Args>
void Logger::debug(const std::string& tag, const std::string& format, Args... args) {
    std::string message = formatString(format, args...);
    debug(tag, message);
}

template<typename... Args>
void Logger::warn(const std::string& tag, const std::string& format, Args... args) {
    std::string message = formatString(format, args...);
    warn(tag, message);
}
