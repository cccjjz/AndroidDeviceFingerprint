# Native层目录结构说明

## 目录结构

```
cpp/
├── include/                    # 头文件目录
│   ├── BaseCollector.h        # 基础收集器抽象类
│   ├── CommonCollector.h      # 通用设备信息收集器
│   ├── Logger.h               # 日志工具类
│   └── SystemCollector.h      # 系统信息收集器
├── src/                       # 源代码目录
│   ├── BaseCollector.cpp      # 基础收集器实现
│   └── native-lib.cpp         # JNI接口主文件
├── utils/                     # 工具类目录
│   └── Logger.cpp             # 日志工具实现
├── collectors/                # 收集器目录
│   ├── SystemCollector/       # 系统信息收集器
│   │   └── SystemCollector.cpp
│   └── CommonCollector/       # 通用信息收集器
│       └── CommonCollector.cpp
├── CMakeLists.txt             # CMake构建文件
└── README.md                  # 本说明文件
```

## 模块说明

### 1. Logger (日志工具)
- **位置**: `include/Logger.h`, `utils/Logger.cpp`
- **功能**: 提供统一的日志输出接口
- **特性**: 支持不同级别的日志输出，格式化字符串支持

### 2. BaseCollector (基础收集器)
- **位置**: `include/BaseCollector.h`, `src/BaseCollector.cpp`
- **功能**: 所有收集器的基类，提供通用工具方法
- **特性**: 文件操作、Base64编码、Java属性获取等

### 3. SystemCollector (系统信息收集器)
- **位置**: `include/SystemCollector.h`, `collectors/SystemCollector/SystemCollector.cpp`
- **功能**: 收集系统级别的设备指纹信息
- **包含信息**:
  - 文件系统信息
  - DRM ID
  - 内核文件信息
  - 系统文件信息

### 4. CommonCollector (通用信息收集器)
- **位置**: `include/CommonCollector.h`, `collectors/CommonCollector/CommonCollector.cpp`
- **功能**: 收集通用的设备信息
- **包含信息**:
  - 设备基本信息
  - 网络信息
  - 硬件信息
  - 应用信息

### 5. native-lib.cpp (JNI接口)
- **位置**: `src/native-lib.cpp`
- **功能**: 提供JNI接口，调用各个收集器
- **接口方法**:
  - `getFileSystemInfoNative()` - 获取文件系统信息
  - `getDrmIdNative()` - 获取DRM ID
  - `getKernelFilesInfoNative()` - 获取内核文件信息
  - `getSystemFilesInfoNative()` - 获取系统文件信息
  - `getCommonDeviceInfoNative()` - 获取通用设备信息
  - `getAllDeviceFingerprintNative()` - 获取所有设备指纹信息

## 构建说明

CMakeLists.txt已更新以支持新的目录结构：
- 自动收集所有源文件
- 设置正确的包含目录
- 保持原有的链接库配置

## 使用示例

```cpp
// 在JNI函数中使用
SystemCollector systemCollector(env);
std::string result = systemCollector.collect();

CommonCollector commonCollector(env);
std::string deviceInfo = commonCollector.collect();
```

## 扩展说明

如需添加新的收集器：
1. 在`include/`目录下创建头文件
2. 在`collectors/`目录下创建对应的实现文件
3. 继承`BaseCollector`类
4. 在`native-lib.cpp`中添加对应的JNI接口
