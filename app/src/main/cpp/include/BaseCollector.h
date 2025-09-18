#ifndef BASE_COLLECTOR_H
#define BASE_COLLECTOR_H

#include <string>
#include <jni.h>

class BaseCollector {
public:
    virtual ~BaseCollector() = default;
    
    // 纯虚函数，子类必须实现
    virtual std::string collect() = 0;
    virtual std::string getCollectorName() const = 0;
    
    // 通用工具方法
protected:
    static bool fileExists(const char* filepath);
    static std::string readFile(const char* filepath);
    static std::string base64Encode(const uint8_t* data, size_t length);
    static std::string executeCommand(const char* command);
    
    // JNI相关工具方法
    static std::string getJavaProperty(JNIEnv* env, const std::string& propertyName);
    static std::string getJavaSystemProperty(JNIEnv* env, const std::string& propertyName);
};

#endif // BASE_COLLECTOR_H
