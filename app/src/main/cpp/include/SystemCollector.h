#ifndef SYSTEM_COLLECTOR_H
#define SYSTEM_COLLECTOR_H

#include "BaseCollector.h"
#include <jni.h>

class SystemCollector : public BaseCollector {
public:
    SystemCollector(JNIEnv* env);
    virtual ~SystemCollector() = default;
    
    std::string collect() override;
    std::string getCollectorName() const override;
    
    // 系统信息收集方法
    std::string collectFileSystemInfo();
    std::string collectDrmId();
    std::string collectKernelFilesInfo();
    std::string collectSystemFilesInfo();
    
private:
    JNIEnv* m_env;
    
    // 辅助方法
    std::string parseBuildProp(const std::string& content, const std::string& filepath);
    std::string getUnameInfo();
    std::string collectBuildPropFiles();
    std::string collectSystemFiles();
    std::string collectAdditionalSystemInfo();
};

#endif // SYSTEM_COLLECTOR_H
