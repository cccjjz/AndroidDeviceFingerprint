#include <jni.h>
#include <string>
#include "../include/Logger.h"
#include "../include/SystemCollector.h"
#include "../include/CommonCollector.h"
#include "ifaddrs.h"
#include <linux/if_packet.h>
#include <netdb.h>
#include <net/if.h>
#include <cstdio>

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
    
    LOGI("NativeLib", "Starting file system info collection...");
    
    try {
        SystemCollector systemCollector(env);
        std::string result = systemCollector.collectFileSystemInfo();
        
        LOGI("NativeLib", "File system info collection completed");
        return env->NewStringUTF(result.c_str());
        
    } catch (const std::exception& e) {
        LOGE("NativeLib", "Exception in getFileSystemInfoNative: %s", e.what());
        return env->NewStringUTF(("Unable to retrieve: " + std::string(e.what())).c_str());
    } catch (...) {
        LOGE("NativeLib", "Unknown exception in getFileSystemInfoNative");
        return env->NewStringUTF("Unable to retrieve: Unknown exception occurred");
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_android_androiddevicefingerprint_MainActivity_getDrmIdNative(
        JNIEnv* env,
        jobject /* this */) {
    
    LOGI("NativeLib", "Starting DRM ID collection...");
    
    try {
        SystemCollector systemCollector(env);
        std::string result = systemCollector.collectDrmId();
        
        LOGI("NativeLib", "DRM ID collection completed");
        return env->NewStringUTF(result.c_str());
        
    } catch (const std::exception& e) {
        LOGE("NativeLib", "Exception in getDrmIdNative: %s", e.what());
        return env->NewStringUTF(("Unable to retrieve: " + std::string(e.what())).c_str());
    } catch (...) {
        LOGE("NativeLib", "Unknown exception in getDrmIdNative");
        return env->NewStringUTF("Unable to retrieve: Unknown exception occurred");
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_android_androiddevicefingerprint_MainActivity_getKernelFilesInfoNative(
        JNIEnv* env,
        jobject /* this */) {
    
    LOGI("NativeLib", "Starting kernel files info collection...");
    
    try {
        SystemCollector systemCollector(env);
        std::string result = systemCollector.collectKernelFilesInfo();
        
        LOGI("NativeLib", "Kernel files info collection completed");
        return env->NewStringUTF(result.c_str());
        
    } catch (const std::exception& e) {
        LOGE("NativeLib", "Exception in getKernelFilesInfoNative: %s", e.what());
        return env->NewStringUTF(("Unable to retrieve: " + std::string(e.what())).c_str());
    } catch (...) {
        LOGE("NativeLib", "Unknown exception in getKernelFilesInfoNative");
        return env->NewStringUTF("Unable to retrieve: Unknown exception occurred");
    }
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_android_androiddevicefingerprint_MainActivity_getSystemFilesInfoNative(
        JNIEnv* env,
        jobject /* this */) {
    
    LOGI("NativeLib", "Starting system files info collection...");
    
    try {
        SystemCollector systemCollector(env);
        std::string result = systemCollector.collectSystemFilesInfo();
        
        LOGI("NativeLib", "System files info collection completed");
        return env->NewStringUTF(result.c_str());
        
    } catch (const std::exception& e) {
        LOGE("NativeLib", "Exception in getSystemFilesInfoNative: %s", e.what());
        return env->NewStringUTF(("Unable to retrieve: " + std::string(e.what())).c_str());
    } catch (...) {
        LOGE("NativeLib", "Unknown exception in getSystemFilesInfoNative");
        return env->NewStringUTF("Unable to retrieve: Unknown exception occurred");
    }
}

// 新增：获取通用设备信息
extern "C" JNIEXPORT jstring JNICALL
Java_com_android_androiddevicefingerprint_MainActivity_getCommonDeviceInfoNative(
        JNIEnv* env,
        jobject /* this */) {
    
    LOGI("NativeLib", "Starting common device info collection...");
    
    try {
        CommonCollector commonCollector(env);
        std::string result = commonCollector.collect();
        
        LOGI("NativeLib", "Common device info collection completed");
        return env->NewStringUTF(result.c_str());
        
    } catch (const std::exception& e) {
        LOGE("NativeLib", "Exception in getCommonDeviceInfoNative: %s", e.what());
        return env->NewStringUTF(("Unable to retrieve: " + std::string(e.what())).c_str());
    } catch (...) {
        LOGE("NativeLib", "Unknown exception in getCommonDeviceInfoNative");
        return env->NewStringUTF("Unable to retrieve: Unknown exception occurred");
    }
}

// 新增：获取所有设备指纹信息
extern "C" JNIEXPORT jstring JNICALL
Java_com_android_androiddevicefingerprint_MainActivity_getAllDeviceFingerprintNative(
        JNIEnv* env,
        jobject /* this */) {
    
    LOGI("NativeLib", "Starting comprehensive device fingerprint collection...");
    
    try {
        std::string result = "=== Comprehensive Device Fingerprint Collection ===\n\n";
        
        // 收集系统信息
        SystemCollector systemCollector(env);
        result += systemCollector.collect();
        
        // 收集通用设备信息
        CommonCollector commonCollector(env);
        result += commonCollector.collect();
        
        LOGI("NativeLib", "Comprehensive device fingerprint collection completed");
        return env->NewStringUTF(result.c_str());
        
    } catch (const std::exception& e) {
        LOGE("NativeLib", "Exception in getAllDeviceFingerprintNative: %s", e.what());
        return env->NewStringUTF(("Unable to retrieve: " + std::string(e.what())).c_str());
    } catch (...) {
        LOGE("NativeLib", "Unknown exception in getAllDeviceFingerprintNative");
        return env->NewStringUTF("Unable to retrieve: Unknown exception occurred");
    }
}

// 简化版本的 MAC 地址获取函数
int listmacaddrs() {
    struct ifaddrs *ifap, *ifaptr;

    if (myGetifaddrs(&ifap) == 0) {
        for (ifaptr = ifap; ifaptr != nullptr; ifaptr = (ifaptr)->ifa_next) {
            if(ifaptr->ifa_addr!= nullptr) {
                sa_family_t family = ((ifaptr)->ifa_addr)->sa_family;
                if (family == AF_PACKET) {
                    //get mac info
                    char macp[INET6_ADDRSTRLEN];
                    auto *sockadd = (struct sockaddr_ll *) (ifaptr->ifa_addr);
                    int i;
                    int len = 0;
                    for (i = 0; i < 6; i++) {
                        len += sprintf(macp + len, "%02X%s", sockadd->sll_addr[i],( i < 5 ? ":" : ""));
                    }
                    LOGE("AF_PACKET  %s  %s  ",(ifaptr)->ifa_name,macp);
                } else if(family == AF_INET || family == AF_INET6){
                    //get v4 & v6 info
                    char host[NI_MAXHOST];
                    int ret = getnameinfo(ifaptr->ifa_addr,
                                    (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                    sizeof(struct sockaddr_in6),
                                    host, NI_MAXHOST,
                                    nullptr, 0, NI_NUMERICHOST);
                    if (ret != 0) {
                        LOGE("AF_INET6  getnameinfo() failed   %s  ",gai_strerror(ret));
                    }
                    LOGE("AF_INET6 %s %s  ",(ifaptr)->ifa_name,host);
                }
            }
        }
        freeifaddrs(ifap);
        return 0;
    } else {
        return 0;
    }
}

// 新增：使用 bionic_netlink 方式获取 MAC 地址
extern "C" JNIEXPORT void JNICALL
Java_com_android_androiddevicefingerprint_MainActivity_getmac(
        JNIEnv* env,
        jclass clazz) {
    
    LOGI("NativeLib", "Starting MAC address collection using bionic netlink...");
    
    try {
        int result = listmacaddrs();
        LOGI("NativeLib", "MAC address collection completed with result: %d", result);
        
    } catch (const std::exception& e) {
        LOGE("NativeLib", "Exception in getmac: %s", e.what());
    } catch (...) {
        LOGE("NativeLib", "Unknown exception in getmac");
    }
}

// 新增：获取 MAC 地址信息并返回字符串
extern "C" JNIEXPORT jstring JNICALL
Java_com_android_androiddevicefingerprint_MainActivity_getMacAddressInfoNative(
        JNIEnv* env,
        jobject /* this */) {
    
    LOGI("NativeLib", "Starting MAC address info collection...");
    
    try {
        int result = listmacaddrs();
        std::string resultStr = "MAC address collection completed with result: " + std::to_string(result);
        
        LOGI("NativeLib", "MAC address info collection completed");
        return env->NewStringUTF(resultStr.c_str());
        
    } catch (const std::exception& e) {
        LOGE("NativeLib", "Exception in getMacAddressInfoNative: %s", e.what());
        return env->NewStringUTF(("Unable to retrieve: " + std::string(e.what())).c_str());
    } catch (...) {
        LOGE("NativeLib", "Unknown exception in getMacAddressInfoNative");
        return env->NewStringUTF("Unable to retrieve: Unknown exception occurred");
    }
}
