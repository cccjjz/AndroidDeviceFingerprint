#include "../../include/SystemCollector.h"
#include "../../include/Logger.h"
#include <sys/statfs.h>
#include <cstdio>
#include <cstdlib>
#include <media/NdkMediaDrm.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <sstream>
#include <sys/utsname.h>
#include <algorithm>

SystemCollector::SystemCollector(JNIEnv* env) : m_env(env) {
}

std::string SystemCollector::collect() {
    std::string result = "=== System Information Collection ===\n\n";
    
    try {
        result += collectFileSystemInfo();
        result += collectDrmId();
        result += collectKernelFilesInfo();
        result += collectSystemFilesInfo();
    } catch (const std::exception& e) {
        LOGE("SystemCollector", "Exception in collect: %s", e.what());
        result += "Error: " + std::string(e.what()) + "\n";
    }
    
    return result;
}

std::string SystemCollector::getCollectorName() const {
    return "SystemCollector";
}

std::string SystemCollector::collectFileSystemInfo() {
    std::string result = "=== File System Information ===\n\n";
    
    // Method 1: Using Java StatFs
    try {
        jclass statFsClass = m_env->FindClass("android/os/StatFs");
        if (statFsClass != nullptr) {
            jmethodID constructor = m_env->GetMethodID(statFsClass, "<init>", "(Ljava/lang/String;)V");
            jmethodID getTotalBytes = m_env->GetMethodID(statFsClass, "getTotalBytes", "()J");
            jmethodID getFreeBytes = m_env->GetMethodID(statFsClass, "getFreeBytes", "()J");
            jmethodID getAvailableBytes = m_env->GetMethodID(statFsClass, "getAvailableBytes", "()J");
            
            if (constructor && getTotalBytes && getFreeBytes && getAvailableBytes) {
                jstring path = m_env->NewStringUTF("/storage/emulated/0");
                jobject statFs = m_env->NewObject(statFsClass, constructor, path);
                
                jlong totalBytes = m_env->CallLongMethod(statFs, getTotalBytes);
                jlong freeBytes = m_env->CallLongMethod(statFs, getFreeBytes);
                jlong availableBytes = m_env->CallLongMethod(statFs, getAvailableBytes);
                
                result += "Java StatFs Method:\n";
                result += "Total Bytes: " + std::to_string(totalBytes) + "\n";
                result += "Free Bytes: " + std::to_string(freeBytes) + "\n";
                result += "Available Bytes: " + std::to_string(availableBytes) + "\n\n";
                
                m_env->DeleteLocalRef(statFs);
                m_env->DeleteLocalRef(path);
            }
        }
        m_env->DeleteLocalRef(statFsClass);
    } catch (...) {
        result += "Java StatFs Method: Failed\n\n";
    }
    
    // Method 2: Using stat command
    result += "stat command output:\n";
    char buffer[1024];
    FILE *fp = popen("stat -f /storage/emulated/0", "r");
    if (fp != nullptr) {
        while (fgets(buffer, sizeof(buffer), fp) != nullptr) {
            result += buffer;
        }
        pclose(fp);
    } else {
        result += "Failed to execute stat command\n";
    }
    result += "\n";
    
    // Method 3: Using statfs64 system call
    result += "statfs64 system call:\n";
    struct statfs64 buf = {};
    if (statfs64("/storage/emulated/0", &buf) == 0) {
        result += "File System Type: " + std::to_string(buf.f_type) + "\n";
        result += "Block Size: " + std::to_string(buf.f_bsize) + "\n";
        result += "Total Blocks: " + std::to_string(buf.f_blocks) + "\n";
        result += "Free Blocks: " + std::to_string(buf.f_bfree) + "\n";
        result += "Available Blocks: " + std::to_string(buf.f_bavail) + "\n";
        result += "Total File Nodes: " + std::to_string(buf.f_files) + "\n";
        result += "Free File Nodes: " + std::to_string(buf.f_ffree) + "\n";
        result += "File System ID: " + std::to_string(buf.f_fsid.__val[0]) + ", " + std::to_string(buf.f_fsid.__val[1]) + "\n";
        result += "Max Filename Length: " + std::to_string(buf.f_namelen) + "\n";
    } else {
        result += "statfs64 system call failed\n";
    }
    
    return result;
}

std::string SystemCollector::collectDrmId() {
    std::string result = "\n=== DRM ID Information ===\n\n";
    
    LOGI("SystemCollector", "Starting DRM ID retrieval...");
    
    try {
        // Widevine DRM UUID
        const uint8_t uuid[] = {0xed, 0xef, 0x8b, 0xa9, 0x79, 0xd6, 0x4a, 0xce,
                               0xa3, 0xc8, 0x27, 0xdc, 0xd5, 0x1d, 0x21, 0xed};
        
        // 创建MediaDrm实例
        AMediaDrm* mediaDrm = AMediaDrm_createByUUID(uuid);
        if (mediaDrm == nullptr) {
            LOGE("SystemCollector", "Failed to create MediaDrm instance");
            return result + "Unable to retrieve: Failed to create MediaDrm instance\n";
        }
        
        // 获取设备唯一ID
        AMediaDrmByteArray deviceUniqueId;
        media_status_t status = AMediaDrm_getPropertyByteArray(mediaDrm, 
                                                              PROPERTY_DEVICE_UNIQUE_ID, 
                                                              &deviceUniqueId);
        
        if (status != AMEDIA_OK) {
            LOGE("SystemCollector", "Failed to get device unique ID, status: %d", status);
            AMediaDrm_release(mediaDrm);
            return result + "Unable to retrieve: Failed to get device unique ID\n";
        }
        
        if (deviceUniqueId.ptr == nullptr || deviceUniqueId.length == 0) {
            LOGE("SystemCollector", "Device unique ID is null or empty");
            AMediaDrm_release(mediaDrm);
            return result + "Unable to retrieve: Device unique ID is null or empty\n";
        }
        
        // Base64编码
        std::string encodedId = base64Encode(deviceUniqueId.ptr, deviceUniqueId.length);
        
        LOGI("SystemCollector", "DRM ID retrieved successfully, length: %zu", deviceUniqueId.length);
        
        // 清理资源
        AMediaDrm_release(mediaDrm);
        
        return result + "DRM ID: " + encodedId + "\n";
        
    } catch (const std::exception& e) {
        LOGE("SystemCollector", "Exception in collectDrmId: %s", e.what());
        return result + "Unable to retrieve: " + std::string(e.what()) + "\n";
    } catch (...) {
        LOGE("SystemCollector", "Unknown exception in collectDrmId");
        return result + "Unable to retrieve: Unknown exception occurred\n";
    }
}

std::string SystemCollector::collectKernelFilesInfo() {
    std::string result = "\n=== Kernel Files Information ===\n\n";
    
    LOGI("SystemCollector", "Starting kernel files info retrieval...");
    
    try {
        result += collectBuildPropFiles();
        
        // 添加其他重要的系统文件
        std::vector<std::string> other_files = {
            "/proc/version",
            "/proc/cpuinfo",
            "/proc/meminfo",
            "/system/etc/prop.default"
        };
        
        result += "=== Other System Files ===\n";
        for (const auto& filepath : other_files) {
            LOGI("SystemCollector", "Reading file: %s", filepath.c_str());
            
            if (fileExists(filepath.c_str())) {
                std::string content = readFile(filepath.c_str());
                result += "--- " + filepath + " ---\n";
                if (content.length() > 1000) {
                    // 截取前1000个字符
                    result += content.substr(0, 1000) + "...\n";
                } else {
                    result += content + "\n";
                }
                result += "\n";
            } else {
                result += "--- " + filepath + " ---\n";
                result += "File does not exist\n\n";
            }
        }
        
        LOGI("SystemCollector", "Kernel files info retrieval completed");
        return result;
        
    } catch (const std::exception& e) {
        LOGE("SystemCollector", "Exception in collectKernelFilesInfo: %s", e.what());
        return result + "Unable to retrieve: " + std::string(e.what()) + "\n";
    } catch (...) {
        LOGE("SystemCollector", "Unknown exception in collectKernelFilesInfo");
        return result + "Unable to retrieve: Unknown exception occurred\n";
    }
}

std::string SystemCollector::collectSystemFilesInfo() {
    std::string result = "\n=== System Files Information (Important Device Fingerprints) ===\n\n";
    
    LOGI("SystemCollector", "Starting system files info retrieval...");
    
    try {
        result += collectSystemFiles();
        result += getUnameInfo();
        result += collectAdditionalSystemInfo();
        
        LOGI("SystemCollector", "System files info retrieval completed");
        return result;
        
    } catch (const std::exception& e) {
        LOGE("SystemCollector", "Exception in collectSystemFilesInfo: %s", e.what());
        return result + "Unable to retrieve: " + std::string(e.what()) + "\n";
    } catch (...) {
        LOGE("SystemCollector", "Unknown exception in collectSystemFilesInfo");
        return result + "Unable to retrieve: Unknown exception occurred\n";
    }
}

std::string SystemCollector::parseBuildProp(const std::string& content, const std::string& filepath) {
    std::string result = "=== " + filepath + " ===\n";
    
    if (content.empty()) {
        result += "File is empty or could not be read\n\n";
        return result;
    }
    
    // 关键属性列表
    std::vector<std::string> key_properties = {
        "ro.build.fingerprint",
        "ro.build.display.id",
        "ro.build.version.release",
        "ro.build.version.sdk",
        "ro.build.version.codename",
        "ro.build.version.incremental",
        "ro.build.date",
        "ro.build.date.utc",
        "ro.build.type",
        "ro.build.user",
        "ro.build.host",
        "ro.build.tags",
        "ro.product.model",
        "ro.product.brand",
        "ro.product.name",
        "ro.product.device",
        "ro.product.manufacturer",
        "ro.product.cpu.abi",
        "ro.product.cpu.abilist",
        "ro.product.locale",
        "ro.board.platform",
        "ro.build.id",
        "ro.build.version.security_patch",
        "ro.build.version.base_os",
        "ro.build.version.preview_sdk",
        "ro.build.version.min_supported_target_sdk"
    };
    
    std::istringstream stream(content);
    std::string line;
    std::vector<std::string> found_properties;
    
    while (std::getline(stream, line)) {
        // 跳过注释和空行
        if (line.empty() || line[0] == '#') continue;
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // 检查是否是关键属性
            for (const auto& prop : key_properties) {
                if (key == prop) {
                    found_properties.push_back(key + "=" + value);
                    break;
                }
            }
        }
    }
    
    if (found_properties.empty()) {
        result += "No key properties found\n";
    } else {
        for (const auto& prop : found_properties) {
            result += prop + "\n";
        }
    }
    
    result += "\n";
    return result;
}

std::string SystemCollector::getUnameInfo() {
    std::string result = "=== uname system call (Android 11+ fallback) ===\n";
    
    try {
        struct utsname buff;
        int ret = uname(&buff);
        
        if (ret == 0) {
            result += "sysname: " + std::string(buff.sysname) + "\n";
            result += "nodename: " + std::string(buff.nodename) + "\n";
            result += "release: " + std::string(buff.release) + "\n";
            result += "version: " + std::string(buff.version) + "\n";
            result += "machine: " + std::string(buff.machine) + "\n";
            result += "domainname: " + std::string(buff.domainname) + "\n";
        } else {
            result += "uname system call failed, errno: " + std::to_string(errno) + "\n";
        }
    } catch (const std::exception& e) {
        result += "Exception in uname: " + std::string(e.what()) + "\n";
    }
    
    result += "\n";
    return result;
}

std::string SystemCollector::collectBuildPropFiles() {
    std::string result;
    
    // 四个主要的build.prop文件路径
    std::vector<std::string> build_prop_files = {
        "/system/build.prop",
        "/odm/etc/build.prop", 
        "/product/build.prop",
        "/vendor/build.prop"
    };
    
    for (const auto& filepath : build_prop_files) {
        LOGI("SystemCollector", "Reading file: %s", filepath.c_str());
        
        if (fileExists(filepath.c_str())) {
            std::string content = readFile(filepath.c_str());
            result += parseBuildProp(content, filepath);
        } else {
            result += "=== " + filepath + " ===\n";
            result += "File does not exist\n\n";
        }
    }
    
    return result;
}

std::string SystemCollector::collectSystemFiles() {
    std::string result;
    
    // 重要的系统文件列表
    std::vector<std::string> system_files = {
        "/proc/sys/kernel/random/boot_id",
        "/proc/sys/kernel/random/uuid", 
        "/sys/block/mmcblk0/device/cid",
        "/sys/devices/soc0/serial_number",
        "/proc/misc",
        "/proc/version"
    };
    
    for (const auto& filepath : system_files) {
        LOGI("SystemCollector", "Reading system file: %s", filepath.c_str());
        
        result += "=== " + filepath + " ===\n";
        
        if (fileExists(filepath.c_str())) {
            std::string content = readFile(filepath.c_str());
            if (content.empty() || content.find("Unable to read") != std::string::npos) {
                result += "File exists but could not be read\n";
            } else {
                // 清理内容，移除换行符
                std::string clean_content = content;
                clean_content.erase(std::remove(clean_content.begin(), clean_content.end(), '\n'), clean_content.end());
                clean_content.erase(std::remove(clean_content.begin(), clean_content.end(), '\r'), clean_content.end());
                result += "Content: " + clean_content + "\n";
            }
        } else {
            result += "File does not exist\n";
        }
        result += "\n";
    }
    
    return result;
}

std::string SystemCollector::collectAdditionalSystemInfo() {
    std::string result = "=== Additional System Information ===\n";
    
    // 添加一些额外的系统信息文件
    std::vector<std::string> additional_files = {
        "/proc/cmdline",
        "/proc/cpuinfo",
        "/proc/meminfo",
        "/sys/class/dmi/id/product_uuid",
        "/sys/class/dmi/id/board_serial",
        "/sys/class/dmi/id/chassis_serial"
    };
    
    for (const auto& filepath : additional_files) {
        LOGI("SystemCollector", "Reading additional file: %s", filepath.c_str());
        
        if (fileExists(filepath.c_str())) {
            std::string content = readFile(filepath.c_str());
            result += "--- " + filepath + " ---\n";
            if (content.length() > 500) {
                // 截取前500个字符
                result += content.substr(0, 500) + "...\n";
            } else {
                result += content + "\n";
            }
            result += "\n";
        }
    }
    
    return result;
}
