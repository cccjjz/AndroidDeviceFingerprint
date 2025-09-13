#include <jni.h>
#include <string>
#include <android/log.h>
#include <sys/statfs.h>
#include <cstdio>
#include <cstdlib>

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