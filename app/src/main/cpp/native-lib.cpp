#include <jni.h>
#include <string>
#include <android/log.h>
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

#define LOG_TAG "FileSystemInfo"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL
Java_com_android_androiddevicefingerprint_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_android_androiddevicefingerprint_MainActivity_getFileSystemInfoNative(
        JNIEnv* env,
        jobject /* this */) {
    
    std::string result = "File System Information:\n\n";
    
    // Method 1: Using Java StatFs
    try {
        jclass statFsClass = env->FindClass("android/os/StatFs");
        if (statFsClass != nullptr) {
            jmethodID constructor = env->GetMethodID(statFsClass, "<init>", "(Ljava/lang/String;)V");
            jmethodID getTotalBytes = env->GetMethodID(statFsClass, "getTotalBytes", "()J");
            jmethodID getFreeBytes = env->GetMethodID(statFsClass, "getFreeBytes", "()J");
            jmethodID getAvailableBytes = env->GetMethodID(statFsClass, "getAvailableBytes", "()J");
            
            if (constructor && getTotalBytes && getFreeBytes && getAvailableBytes) {
                jstring path = env->NewStringUTF("/storage/emulated/0");
                jobject statFs = env->NewObject(statFsClass, constructor, path);
                
                jlong totalBytes = env->CallLongMethod(statFs, getTotalBytes);
                jlong freeBytes = env->CallLongMethod(statFs, getFreeBytes);
                jlong availableBytes = env->CallLongMethod(statFs, getAvailableBytes);
                
                result += "Java StatFs Method:\n";
                result += "Total Bytes: " + std::to_string(totalBytes) + "\n";
                result += "Free Bytes: " + std::to_string(freeBytes) + "\n";
                result += "Available Bytes: " + std::to_string(availableBytes) + "\n\n";
                
                env->DeleteLocalRef(statFs);
                env->DeleteLocalRef(path);
            }
        }
        env->DeleteLocalRef(statFsClass);
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
    
    return env->NewStringUTF(result.c_str());
}

// Base64编码函数
std::string base64_encode(const uint8_t* data, size_t length) {
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

extern "C" JNIEXPORT jstring JNICALL
Java_com_android_androiddevicefingerprint_MainActivity_getDrmIdNative(
        JNIEnv* env,
        jobject /* this */) {
    
    LOGI("Starting DRM ID retrieval...");
    
    try {
        // Widevine DRM UUID
        const uint8_t uuid[] = {0xed, 0xef, 0x8b, 0xa9, 0x79, 0xd6, 0x4a, 0xce,
                               0xa3, 0xc8, 0x27, 0xdc, 0xd5, 0x1d, 0x21, 0xed};
        
        // 创建MediaDrm实例
        AMediaDrm* mediaDrm = AMediaDrm_createByUUID(uuid);
        if (mediaDrm == nullptr) {
            LOGE("Failed to create MediaDrm instance");
            return env->NewStringUTF("Unable to retrieve: Failed to create MediaDrm instance");
        }
        
        // 获取设备唯一ID
        AMediaDrmByteArray deviceUniqueId;
        media_status_t status = AMediaDrm_getPropertyByteArray(mediaDrm, 
                                                              PROPERTY_DEVICE_UNIQUE_ID, 
                                                              &deviceUniqueId);
        
        if (status != AMEDIA_OK) {
            LOGE("Failed to get device unique ID, status: %d", status);
            AMediaDrm_release(mediaDrm);
            return env->NewStringUTF("Unable to retrieve: Failed to get device unique ID");
        }
        
        if (deviceUniqueId.ptr == nullptr || deviceUniqueId.length == 0) {
            LOGE("Device unique ID is null or empty");
            AMediaDrm_release(mediaDrm);
            return env->NewStringUTF("Unable to retrieve: Device unique ID is null or empty");
        }
        
        // Base64编码
        std::string encodedId = base64_encode(deviceUniqueId.ptr, deviceUniqueId.length);
        
        LOGI("DRM ID retrieved successfully, length: %zu", deviceUniqueId.length);
        
        // 清理资源
        AMediaDrm_release(mediaDrm);
        
        return env->NewStringUTF(encodedId.c_str());
        
    } catch (const std::exception& e) {
        LOGE("Exception in getDrmIdNative: %s", e.what());
        return env->NewStringUTF(("Unable to retrieve: " + std::string(e.what())).c_str());
    } catch (...) {
        LOGE("Unknown exception in getDrmIdNative");
        return env->NewStringUTF("Unable to retrieve: Unknown exception occurred");
    }
}

// 自定义文件读取函数，替代系统的cat/popen
std::string custom_read_file(const char* filepath) {
    std::string result;
    int fd = -1;
    
    try {
        // 使用openat系统调用，避免SVC重定向
        fd = open(filepath, O_RDONLY);
        if (fd == -1) {
            LOGE("Failed to open file: %s, errno: %d", filepath, errno);
            return "Unable to read file: " + std::string(filepath);
        }
        
        // 获取文件大小
        struct stat file_stat;
        if (fstat(fd, &file_stat) == -1) {
            LOGE("Failed to get file stat: %s", filepath);
            close(fd);
            return "Unable to get file stat: " + std::string(filepath);
        }
        
        // 读取文件内容
        std::vector<char> buffer(file_stat.st_size + 1);
        ssize_t bytes_read = read(fd, buffer.data(), file_stat.st_size);
        
        if (bytes_read == -1) {
            LOGE("Failed to read file: %s, errno: %d", filepath, errno);
            close(fd);
            return "Unable to read file content: " + std::string(filepath);
        }
        
        buffer[bytes_read] = '\0';
        result = std::string(buffer.data());
        
        close(fd);
        return result;
        
    } catch (const std::exception& e) {
        if (fd != -1) close(fd);
        LOGE("Exception in custom_read_file: %s", e.what());
        return "Exception reading file: " + std::string(filepath);
    }
}

// 检查文件是否存在
bool file_exists(const char* filepath) {
    struct stat buffer;
    return (stat(filepath, &buffer) == 0);
}

// 解析build.prop文件，提取关键信息
std::string parse_build_prop(const std::string& content, const std::string& filepath) {
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

extern "C" JNIEXPORT jstring JNICALL
Java_com_android_androiddevicefingerprint_MainActivity_getKernelFilesInfoNative(
        JNIEnv* env,
        jobject /* this */) {
    
    LOGI("Starting kernel files info retrieval...");
    
    try {
        std::string result = "Kernel Files Information:\n\n";
        
        // 四个主要的build.prop文件路径
        std::vector<std::string> build_prop_files = {
            "/system/build.prop",
            "/odm/etc/build.prop", 
            "/product/build.prop",
            "/vendor/build.prop"
        };
        
        for (const auto& filepath : build_prop_files) {
            LOGI("Reading file: %s", filepath.c_str());
            
            if (file_exists(filepath.c_str())) {
                std::string content = custom_read_file(filepath.c_str());
                result += parse_build_prop(content, filepath);
            } else {
                result += "=== " + filepath + " ===\n";
                result += "File does not exist\n\n";
            }
        }
        
        // 添加其他重要的系统文件
        std::vector<std::string> other_files = {
            "/proc/version",
            "/proc/cpuinfo",
            "/proc/meminfo",
            "/system/etc/prop.default"
        };
        
        result += "=== Other System Files ===\n";
        for (const auto& filepath : other_files) {
            LOGI("Reading file: %s", filepath.c_str());
            
            if (file_exists(filepath.c_str())) {
                std::string content = custom_read_file(filepath.c_str());
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
        
        LOGI("Kernel files info retrieval completed");
        return env->NewStringUTF(result.c_str());
        
    } catch (const std::exception& e) {
        LOGE("Exception in getKernelFilesInfoNative: %s", e.what());
        return env->NewStringUTF(("Unable to retrieve: " + std::string(e.what())).c_str());
    } catch (...) {
        LOGE("Unknown exception in getKernelFilesInfoNative");
        return env->NewStringUTF("Unable to retrieve: Unknown exception occurred");
    }
}

// 使用uname系统调用获取系统信息（Android 11+的替代方案）
std::string get_uname_info() {
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

extern "C" JNIEXPORT jstring JNICALL
Java_com_android_androiddevicefingerprint_MainActivity_getSystemFilesInfoNative(
        JNIEnv* env,
        jobject /* this */) {
    
    LOGI("Starting system files info retrieval...");
    
    try {
        std::string result = "System Files Information (Important Device Fingerprints):\n\n";
        
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
            LOGI("Reading system file: %s", filepath.c_str());
            
            result += "=== " + filepath + " ===\n";
            
            if (file_exists(filepath.c_str())) {
                std::string content = custom_read_file(filepath.c_str());
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
        
        // 添加uname信息作为/proc/version的替代方案
        result += get_uname_info();
        
        // 添加一些额外的系统信息文件
        std::vector<std::string> additional_files = {
            "/proc/cmdline",
            "/proc/cpuinfo",
            "/proc/meminfo",
            "/sys/class/dmi/id/product_uuid",
            "/sys/class/dmi/id/board_serial",
            "/sys/class/dmi/id/chassis_serial"
        };
        
        result += "=== Additional System Information ===\n";
        for (const auto& filepath : additional_files) {
            LOGI("Reading additional file: %s", filepath.c_str());
            
            if (file_exists(filepath.c_str())) {
                std::string content = custom_read_file(filepath.c_str());
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
        
        LOGI("System files info retrieval completed");
        return env->NewStringUTF(result.c_str());
        
    } catch (const std::exception& e) {
        LOGE("Exception in getSystemFilesInfoNative: %s", e.what());
        return env->NewStringUTF(("Unable to retrieve: " + std::string(e.what())).c_str());
    } catch (...) {
        LOGE("Unknown exception in getSystemFilesInfoNative");
        return env->NewStringUTF("Unable to retrieve: Unknown exception occurred");
    }
}