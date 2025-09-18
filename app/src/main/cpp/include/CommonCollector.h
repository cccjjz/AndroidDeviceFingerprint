#ifndef COMMON_COLLECTOR_H
#define COMMON_COLLECTOR_H

#include "BaseCollector.h"
#include <jni.h>

class CommonCollector : public BaseCollector {
public:
    CommonCollector(JNIEnv* env);
    virtual ~CommonCollector() = default;
    
    std::string collect() override;
    std::string getCollectorName() const override;
    
    // 通用信息收集方法
    std::string collectDeviceInfo();
    std::string collectNetworkInfo();
    std::string collectHardwareInfo();
    std::string collectAppInfo();
    
private:
    JNIEnv* m_env;
    
    // 辅助方法
    std::string getDeviceModel();
    std::string getDeviceBrand();
    std::string getAndroidVersion();
    std::string getApiLevel();
    std::string getCpuInfo();
    std::string getMemoryInfo();
    std::string getStorageInfo();
};

#endif // COMMON_COLLECTOR_H
