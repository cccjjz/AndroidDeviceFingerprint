#include "../../include/CommonCollector.h"
#include "../../include/Logger.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <vector>

CommonCollector::CommonCollector(JNIEnv* env) : m_env(env) {
}

std::string CommonCollector::collect() {
    std::string result = "=== Common Device Information Collection ===\n\n";
    
    try {
        result += collectDeviceInfo();
        result += collectNetworkInfo();
        result += collectHardwareInfo();
        result += collectAppInfo();
    } catch (const std::exception& e) {
        LOGE("CommonCollector", "Exception in collect: %s", e.what());
        result += "Error: " + std::string(e.what()) + "\n";
    }
    
    return result;
}

std::string CommonCollector::getCollectorName() const {
    return "CommonCollector";
}

std::string CommonCollector::collectDeviceInfo() {
    std::string result = "=== Device Information ===\n\n";
    
    try {
        result += "Device Model: " + getDeviceModel() + "\n";
        result += "Device Brand: " + getDeviceBrand() + "\n";
        result += "Android Version: " + getAndroidVersion() + "\n";
        result += "API Level: " + getApiLevel() + "\n";
        result += "Manufacturer: " + getJavaProperty(m_env, "ro.product.manufacturer") + "\n";
        result += "Product Name: " + getJavaProperty(m_env, "ro.product.name") + "\n";
        result += "Device Name: " + getJavaProperty(m_env, "ro.product.device") + "\n";
        result += "Build Fingerprint: " + getJavaProperty(m_env, "ro.build.fingerprint") + "\n";
        result += "Build ID: " + getJavaProperty(m_env, "ro.build.id") + "\n";
        result += "Build Type: " + getJavaProperty(m_env, "ro.build.type") + "\n";
        result += "Build Tags: " + getJavaProperty(m_env, "ro.build.tags") + "\n";
        result += "Build Date: " + getJavaProperty(m_env, "ro.build.date") + "\n";
        result += "Security Patch: " + getJavaProperty(m_env, "ro.build.version.security_patch") + "\n";
        
    } catch (const std::exception& e) {
        LOGE("CommonCollector", "Exception in collectDeviceInfo: %s", e.what());
        result += "Error collecting device info: " + std::string(e.what()) + "\n";
    }
    
    result += "\n";
    return result;
}

std::string CommonCollector::collectNetworkInfo() {
    std::string result = "=== Network Information ===\n\n";
    
    try {
        // 收集网络相关信息
        result += "WiFi MAC Address: " + getJavaProperty(m_env, "ro.wifi.channels") + "\n";
        result += "Bluetooth Address: " + getJavaProperty(m_env, "ro.bluetooth.address") + "\n";
        result += "Network Type: " + getJavaProperty(m_env, "ro.telephony.default_network") + "\n";
        
        // 尝试读取网络接口信息
        if (fileExists("/sys/class/net/wlan0/address")) {
            std::string mac = readFile("/sys/class/net/wlan0/address");
            if (!mac.empty()) {
                result += "WLAN0 MAC: " + mac + "\n";
            }
        }
        
        if (fileExists("/sys/class/net/eth0/address")) {
            std::string mac = readFile("/sys/class/net/eth0/address");
            if (!mac.empty()) {
                result += "ETH0 MAC: " + mac + "\n";
            }
        }
        
    } catch (const std::exception& e) {
        LOGE("CommonCollector", "Exception in collectNetworkInfo: %s", e.what());
        result += "Error collecting network info: " + std::string(e.what()) + "\n";
    }
    
    result += "\n";
    return result;
}

std::string CommonCollector::collectHardwareInfo() {
    std::string result = "=== Hardware Information ===\n\n";
    
    try {
        result += getCpuInfo();
        result += getMemoryInfo();
        result += getStorageInfo();
        
        // 其他硬件信息
        result += "Board Platform: " + getJavaProperty(m_env, "ro.board.platform") + "\n";
        result += "CPU ABI: " + getJavaProperty(m_env, "ro.product.cpu.abi") + "\n";
        result += "CPU ABI List: " + getJavaProperty(m_env, "ro.product.cpu.abilist") + "\n";
        result += "Hardware: " + getJavaProperty(m_env, "ro.hardware") + "\n";
        result += "Bootloader: " + getJavaProperty(m_env, "ro.bootloader") + "\n";
        
    } catch (const std::exception& e) {
        LOGE("CommonCollector", "Exception in collectHardwareInfo: %s", e.what());
        result += "Error collecting hardware info: " + std::string(e.what()) + "\n";
    }
    
    result += "\n";
    return result;
}

std::string CommonCollector::collectAppInfo() {
    std::string result = "=== Application Information ===\n\n";
    
    try {
        // 获取应用相关信息
        result += "Package Name: " + getJavaSystemProperty(m_env, "java.class.path") + "\n";
        result += "User Agent: " + getJavaSystemProperty(m_env, "http.agent") + "\n";
        result += "File Encoding: " + getJavaSystemProperty(m_env, "file.encoding") + "\n";
        result += "OS Name: " + getJavaSystemProperty(m_env, "os.name") + "\n";
        result += "OS Version: " + getJavaSystemProperty(m_env, "os.version") + "\n";
        result += "OS Arch: " + getJavaSystemProperty(m_env, "os.arch") + "\n";
        result += "Java Version: " + getJavaSystemProperty(m_env, "java.version") + "\n";
        result += "Java Vendor: " + getJavaSystemProperty(m_env, "java.vendor") + "\n";
        
    } catch (const std::exception& e) {
        LOGE("CommonCollector", "Exception in collectAppInfo: %s", e.what());
        result += "Error collecting app info: " + std::string(e.what()) + "\n";
    }
    
    result += "\n";
    return result;
}

std::string CommonCollector::getDeviceModel() {
    return getJavaProperty(m_env, "ro.product.model");
}

std::string CommonCollector::getDeviceBrand() {
    return getJavaProperty(m_env, "ro.product.brand");
}

std::string CommonCollector::getAndroidVersion() {
    return getJavaProperty(m_env, "ro.build.version.release");
}

std::string CommonCollector::getApiLevel() {
    return getJavaProperty(m_env, "ro.build.version.sdk");
}

std::string CommonCollector::getCpuInfo() {
    std::string result = "=== CPU Information ===\n";
    
    try {
        if (fileExists("/proc/cpuinfo")) {
            std::string content = readFile("/proc/cpuinfo");
            if (!content.empty()) {
                // 提取关键CPU信息
                std::istringstream stream(content);
                std::string line;
                std::vector<std::string> cpu_info;
                
                while (std::getline(stream, line)) {
                    if (line.find("processor") != std::string::npos ||
                        line.find("model name") != std::string::npos ||
                        line.find("Hardware") != std::string::npos ||
                        line.find("CPU architecture") != std::string::npos ||
                        line.find("CPU implementer") != std::string::npos ||
                        line.find("CPU variant") != std::string::npos ||
                        line.find("CPU part") != std::string::npos ||
                        line.find("CPU revision") != std::string::npos) {
                        cpu_info.push_back(line);
                    }
                }
                
                for (const auto& info : cpu_info) {
                    result += info + "\n";
                }
            }
        } else {
            result += "CPU info file not accessible\n";
        }
    } catch (const std::exception& e) {
        LOGE("CommonCollector", "Exception in getCpuInfo: %s", e.what());
        result += "Error reading CPU info: " + std::string(e.what()) + "\n";
    }
    
    result += "\n";
    return result;
}

std::string CommonCollector::getMemoryInfo() {
    std::string result = "=== Memory Information ===\n";
    
    try {
        if (fileExists("/proc/meminfo")) {
            std::string content = readFile("/proc/meminfo");
            if (!content.empty()) {
                // 提取关键内存信息
                std::istringstream stream(content);
                std::string line;
                std::vector<std::string> mem_info;
                
                while (std::getline(stream, line)) {
                    if (line.find("MemTotal") != std::string::npos ||
                        line.find("MemFree") != std::string::npos ||
                        line.find("MemAvailable") != std::string::npos ||
                        line.find("Buffers") != std::string::npos ||
                        line.find("Cached") != std::string::npos ||
                        line.find("SwapTotal") != std::string::npos ||
                        line.find("SwapFree") != std::string::npos) {
                        mem_info.push_back(line);
                    }
                }
                
                for (const auto& info : mem_info) {
                    result += info + "\n";
                }
            }
        } else {
            result += "Memory info file not accessible\n";
        }
    } catch (const std::exception& e) {
        LOGE("CommonCollector", "Exception in getMemoryInfo: %s", e.what());
        result += "Error reading memory info: " + std::string(e.what()) + "\n";
    }
    
    result += "\n";
    return result;
}

std::string CommonCollector::getStorageInfo() {
    std::string result = "=== Storage Information ===\n";
    
    try {
        // 使用Java StatFs获取存储信息
        jclass statFsClass = m_env->FindClass("android/os/StatFs");
        if (statFsClass != nullptr) {
            jmethodID constructor = m_env->GetMethodID(statFsClass, "<init>", "(Ljava/lang/String;)V");
            jmethodID getTotalBytes = m_env->GetMethodID(statFsClass, "getTotalBytes", "()J");
            jmethodID getFreeBytes = m_env->GetMethodID(statFsClass, "getFreeBytes", "()J");
            jmethodID getAvailableBytes = m_env->GetMethodID(statFsClass, "getAvailableBytes", "()J");
            
            if (constructor && getTotalBytes && getFreeBytes && getAvailableBytes) {
                // 内部存储
                jstring internalPath = m_env->NewStringUTF("/data");
                jobject internalStatFs = m_env->NewObject(statFsClass, constructor, internalPath);
                
                jlong internalTotal = m_env->CallLongMethod(internalStatFs, getTotalBytes);
                jlong internalFree = m_env->CallLongMethod(internalStatFs, getFreeBytes);
                jlong internalAvailable = m_env->CallLongMethod(internalStatFs, getAvailableBytes);
                
                result += "Internal Storage:\n";
                result += "  Total: " + std::to_string(internalTotal) + " bytes\n";
                result += "  Free: " + std::to_string(internalFree) + " bytes\n";
                result += "  Available: " + std::to_string(internalAvailable) + " bytes\n";
                
                m_env->DeleteLocalRef(internalStatFs);
                m_env->DeleteLocalRef(internalPath);
                
                // 外部存储
                jstring externalPath = m_env->NewStringUTF("/storage/emulated/0");
                jobject externalStatFs = m_env->NewObject(statFsClass, constructor, externalPath);
                
                jlong externalTotal = m_env->CallLongMethod(externalStatFs, getTotalBytes);
                jlong externalFree = m_env->CallLongMethod(externalStatFs, getFreeBytes);
                jlong externalAvailable = m_env->CallLongMethod(externalStatFs, getAvailableBytes);
                
                result += "External Storage:\n";
                result += "  Total: " + std::to_string(externalTotal) + " bytes\n";
                result += "  Free: " + std::to_string(externalFree) + " bytes\n";
                result += "  Available: " + std::to_string(externalAvailable) + " bytes\n";
                
                m_env->DeleteLocalRef(externalStatFs);
                m_env->DeleteLocalRef(externalPath);
            }
        }
        m_env->DeleteLocalRef(statFsClass);
        
    } catch (const std::exception& e) {
        LOGE("CommonCollector", "Exception in getStorageInfo: %s", e.what());
        result += "Error reading storage info: " + std::string(e.what()) + "\n";
    }
    
    result += "\n";
    return result;
}
