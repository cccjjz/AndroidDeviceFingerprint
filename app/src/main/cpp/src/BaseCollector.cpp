#include "../include/BaseCollector.h"
#include "../include/Logger.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <sstream>

bool BaseCollector::fileExists(const char* filepath) {
    struct stat buffer;
    return (stat(filepath, &buffer) == 0);
}

std::string BaseCollector::readFile(const char* filepath) {
    std::string result;
    int fd = -1;
    
    try {
        // 使用open系统调用，避免SVC重定向
        fd = open(filepath, O_RDONLY);
        if (fd == -1) {
            LOGE("BaseCollector", "Failed to open file: %s, errno: %d", filepath, errno);
            return "Unable to read file: " + std::string(filepath);
        }
        
        // 获取文件大小
        struct stat file_stat;
        if (fstat(fd, &file_stat) == -1) {
            LOGE("BaseCollector", "Failed to get file stat: %s", filepath);
            close(fd);
            return "Unable to get file stat: " + std::string(filepath);
        }
        
        // 读取文件内容
        std::vector<char> buffer(file_stat.st_size + 1);
        ssize_t bytes_read = read(fd, buffer.data(), file_stat.st_size);
        
        if (bytes_read == -1) {
            LOGE("BaseCollector", "Failed to read file: %s, errno: %d", filepath, errno);
            close(fd);
            return "Unable to read file content: " + std::string(filepath);
        }
        
        buffer[bytes_read] = '\0';
        result = std::string(buffer.data());
        
        close(fd);
        return result;
        
    } catch (const std::exception& e) {
        if (fd != -1) close(fd);
        LOGE("BaseCollector", "Exception in readFile: %s", e.what());
        return "Exception reading file: " + std::string(filepath);
    }
}

std::string BaseCollector::base64Encode(const uint8_t* data, size_t length) {
    const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    int val = 0, valb = -6;
    for (size_t i = 0; i < length; ++i) {
        val = (val << 8) + data[i];
        valb += 8;
        while (valb >= 0) {
            result.push_back(chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) result.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (result.size() % 4) result.push_back('=');
    return result;
}

std::string BaseCollector::executeCommand(const char* command) {
    std::string result;
    char buffer[1024];
    FILE *fp = popen(command, "r");
    if (fp != nullptr) {
        while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
            result += buffer;
        }
        pclose(fp);
    } else {
        LOGE("BaseCollector", "Failed to execute command: %s", command);
        result = "Failed to execute command: " + std::string(command);
    }
    return result;
}

std::string BaseCollector::getJavaProperty(JNIEnv* env, const std::string& propertyName) {
    try {
        jclass systemClass = env->FindClass("java/lang/System");
        if (systemClass == nullptr) {
            LOGE("BaseCollector", "Failed to find System class");
            return "Unable to access System class";
        }
        
        jmethodID getPropertyMethod = env->GetStaticMethodID(systemClass, "getProperty", "(Ljava/lang/String;)Ljava/lang/String;");
        if (getPropertyMethod == nullptr) {
            LOGE("BaseCollector", "Failed to find getProperty method");
            env->DeleteLocalRef(systemClass);
            return "Unable to access getProperty method";
        }
        
        jstring propertyNameStr = env->NewStringUTF(propertyName.c_str());
        jstring propertyValue = (jstring)env->CallStaticObjectMethod(systemClass, getPropertyMethod, propertyNameStr);
        
        std::string result;
        if (propertyValue != nullptr) {
            const char* valueStr = env->GetStringUTFChars(propertyValue, nullptr);
            if (valueStr != nullptr) {
                result = std::string(valueStr);
                env->ReleaseStringUTFChars(propertyValue, valueStr);
            }
            env->DeleteLocalRef(propertyValue);
        } else {
            result = "Property not found: " + propertyName;
        }
        
        env->DeleteLocalRef(propertyNameStr);
        env->DeleteLocalRef(systemClass);
        
        return result;
        
    } catch (const std::exception& e) {
        LOGE("BaseCollector", "Exception in getJavaProperty: %s", e.what());
        return "Exception: " + std::string(e.what());
    }
}

std::string BaseCollector::getJavaSystemProperty(JNIEnv* env, const std::string& propertyName) {
    try {
        jclass systemClass = env->FindClass("java/lang/System");
        if (systemClass == nullptr) {
            LOGE("BaseCollector", "Failed to find System class");
            return "Unable to access System class";
        }
        
        jmethodID getPropertyMethod = env->GetStaticMethodID(systemClass, "getProperty", "(Ljava/lang/String;)Ljava/lang/String;");
        if (getPropertyMethod == nullptr) {
            LOGE("BaseCollector", "Failed to find getProperty method");
            env->DeleteLocalRef(systemClass);
            return "Unable to access getProperty method";
        }
        
        jstring propertyNameStr = env->NewStringUTF(propertyName.c_str());
        jstring propertyValue = (jstring)env->CallStaticObjectMethod(systemClass, getPropertyMethod, propertyNameStr);
        
        std::string result;
        if (propertyValue != nullptr) {
            const char* valueStr = env->GetStringUTFChars(propertyValue, nullptr);
            if (valueStr != nullptr) {
                result = std::string(valueStr);
                env->ReleaseStringUTFChars(propertyValue, valueStr);
            }
            env->DeleteLocalRef(propertyValue);
        } else {
            result = "System property not found: " + propertyName;
        }
        
        env->DeleteLocalRef(propertyNameStr);
        env->DeleteLocalRef(systemClass);
        
        return result;
        
    } catch (const std::exception& e) {
        LOGE("BaseCollector", "Exception in getJavaSystemProperty: %s", e.what());
        return "Exception: " + std::string(e.what());
    }
}
