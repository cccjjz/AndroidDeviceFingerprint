#include <jni.h>
#include <string>
#include "../include/Logger.h"
#include "../include/SystemCollector.h"
#include "../include/CommonCollector.h"

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
