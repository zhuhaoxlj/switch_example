# 🌐 Network 模块使用指南

Switch2D Engine 网络模块 - 完整的 HTTP 客户端功能

---

## ✨ 功能特性

- ✅ **HTTP 请求** - GET, POST, PUT, DELETE, HEAD
- ✅ **DNS 解析** - 自动解析域名到 IP 地址
- ✅ **文件下载** - 支持进度回调
- ✅ **自定义请求头** - 完全控制 HTTP 请求
- ✅ **URL 编码/解码** - 工具函数
- ✅ **网络状态检测** - 检查连接状态
- ✅ **同步 API** - 简单易用的同步接口
- ✅ **错误处理** - 详细的错误信息

---

## 📋 快速开始

### 1. 基本使用

```cpp
#include "Switch2D/Switch2D.h"
using namespace Switch2D;

// 在引擎初始化后自动可用
NetworkManager* net = Engine::getInstance().getNetwork();

// 检查网络是否已初始化
if (!net->isInitialized()) {
    DEBUG_LOG("Network not available");
    return;
}

// 发送 GET 请求
HTTPResponse resp = net->get("http://httpbin.org/get");

if (resp.isSuccess()) {
    DEBUG_LOG("Response: %s", resp.getBodyAsString().c_str());
} else {
    DEBUG_LOG("Error: %s", resp.error.c_str());
}
```

### 2. 获取网页源码

```cpp
// 获取网页 HTML
HTTPResponse resp = net->get("http://example.com");

if (resp.isSuccess()) {
    std::string html = resp.getBodyAsString();
    DEBUG_LOG("Page size: %zu bytes", html.length());
    DEBUG_LOG("Content: %s", html.c_str());
}
```

---

## 🔧 详细 API

### HTTP GET 请求

```cpp
// 简单 GET 请求
HTTPResponse resp = net->get("http://api.example.com/data");

// 检查状态码
if (resp.statusCode == 200) {
    DEBUG_LOG("Success!");
} else {
    DEBUG_LOG("Status: %d %s", resp.statusCode, resp.statusMessage.c_str());
}

// 获取响应体
std::string body = resp.getBodyAsString();

// 获取响应头
if (resp.headers.find("Content-Type") != resp.headers.end()) {
    DEBUG_LOG("Content-Type: %s", resp.headers["Content-Type"].c_str());
}
```

### HTTP POST 请求

```cpp
// POST JSON 数据
std::string jsonData = R"({"name": "Switch2D", "version": "1.0"})";
HTTPResponse resp = net->post(
    "http://api.example.com/submit",
    jsonData,
    "application/json"
);

// POST 表单数据
std::string formData = "username=player&score=100";
HTTPResponse resp = net->post(
    "http://api.example.com/score",
    formData,
    "application/x-www-form-urlencoded"
);
```

### 自定义请求

```cpp
// 创建自定义请求
HTTPRequest req("http://api.example.com/data");
req.method = HTTPMethod::PUT;
req.headers["Authorization"] = "Bearer YOUR_TOKEN";
req.headers["Accept"] = "application/json";
req.body = R"({"key": "value"})";
req.timeout = 60;  // 60秒超时

// 发送请求
HTTPResponse resp = net->request(req);
```

---

## 📥 文件下载

### 基本下载

```cpp
bool success = net->downloadFile(
    "http://example.com/file.zip",
    "sdmc:/switch/downloads/file.zip"
);

if (success) {
    DEBUG_LOG("Download complete!");
}
```

### 带进度回调的下载

```cpp
bool success = net->downloadFile(
    "http://example.com/large-file.zip",
    "sdmc:/switch/downloads/file.zip",
    [](size_t downloaded, size_t total) {
        float percent = (total > 0) ? (downloaded * 100.0f / total) : 0;
        DEBUG_LOG("Progress: %.1f%% (%zu / %zu bytes)", 
                  percent, downloaded, total);
    }
);
```

---

## 🌍 DNS 解析

```cpp
// 解析域名
std::string ip = net->resolveHost("example.com");
DEBUG_LOG("example.com resolves to: %s", ip.c_str());

// 已经是 IP 地址会直接返回
std::string ip2 = net->resolveHost("93.184.216.34");
// 返回 "93.184.216.34"
```

---

## 🔗 URL 编码/解码

```cpp
// URL 编码
std::string encoded = net->urlEncode("Hello World!");
// 结果: "Hello%20World%21"

// URL 解码
std::string decoded = net->urlDecode("Hello%20World%21");
// 结果: "Hello World!"

// 构建查询字符串
std::string query = "?name=" + net->urlEncode("张三") + 
                   "&age=25";
```

---

## 🛠️ 配置和设置

### 设置默认请求头

```cpp
// 添加默认请求头（所有请求都会包含）
net->setDefaultHeader("User-Agent", "MyGame/1.0");
net->setDefaultHeader("Accept-Language", "zh-CN");

// 移除默认请求头
net->removeDefaultHeader("User-Agent");
```

### 网络状态检测

```cpp
// 检查网络是否可用
if (net->isConnected()) {
    DEBUG_LOG("Network is available");
} else {
    DEBUG_LOG("No network connection");
}

// 获取本地 IP（简化实现）
std::string localIP = net->getLocalIP();
```

---

## 💡 实际应用示例

### 示例 1：获取天气信息

```cpp
class WeatherChecker {
public:
    void checkWeather(const std::string& city) {
        NetworkManager* net = Engine::getInstance().getNetwork();
        
        // 使用免费天气 API
        std::string url = "http://wttr.in/" + 
                         net->urlEncode(city) + 
                         "?format=j1";
        
        HTTPResponse resp = net->get(url);
        
        if (resp.isSuccess()) {
            DEBUG_LOG("Weather data: %s", resp.getBodyAsString().c_str());
            // 解析 JSON 数据...
        } else {
            DEBUG_LOG("Failed: %s", resp.error.c_str());
        }
    }
};
```

### 示例 2：上传游戏分数

```cpp
class ScoreManager {
public:
    void uploadScore(const std::string& playerName, int score) {
        NetworkManager* net = Engine::getInstance().getNetwork();
        
        // 构建 JSON 数据
        std::ostringstream oss;
        oss << R"({"player":")" << playerName << R"(",)"
            << R"("score":)" << score << "}";
        
        HTTPResponse resp = net->post(
            "http://api.mygame.com/scores",
            oss.str(),
            "application/json"
        );
        
        if (resp.isSuccess()) {
            DEBUG_LOG("Score uploaded! Server response: %s",
                     resp.getBodyAsString().c_str());
        } else {
            DEBUG_LOG("Upload failed: %s", resp.error.c_str());
        }
    }
};
```

### 示例 3：下载游戏资源

```cpp
class ResourceDownloader {
public:
    void downloadAsset(const std::string& assetName) {
        NetworkManager* net = Engine::getInstance().getNetwork();
        
        std::string url = "http://cdn.mygame.com/assets/" + assetName;
        std::string savePath = "sdmc:/switch/mygame/assets/" + assetName;
        
        DEBUG_LOG("Downloading %s...", assetName.c_str());
        
        bool success = net->downloadFile(url, savePath,
            [assetName](size_t downloaded, size_t total) {
                if (total > 0) {
                    float percent = downloaded * 100.0f / total;
                    DEBUG_LOG("%s: %.1f%%", assetName.c_str(), percent);
                }
            }
        );
        
        if (success) {
            DEBUG_LOG("Downloaded %s", assetName.c_str());
        } else {
            DEBUG_LOG("Download failed");
        }
    }
};
```

### 示例 4：检查更新

```cpp
class UpdateChecker {
public:
    struct VersionInfo {
        std::string version;
        std::string downloadUrl;
        std::string changelog;
    };
    
    VersionInfo checkForUpdates(const std::string& currentVersion) {
        NetworkManager* net = Engine::getInstance().getNetwork();
        
        HTTPResponse resp = net->get("http://api.mygame.com/version");
        
        VersionInfo info;
        if (resp.isSuccess()) {
            // 解析版本信息 JSON
            // 这里简化处理，实际应使用 JSON 库
            std::string body = resp.getBodyAsString();
            
            // 假设返回: {"version":"1.2.0","url":"...","changelog":"..."}
            // 实际应用中使用正确的 JSON 解析
            
            info.version = "1.2.0";  // 从 JSON 解析
            info.downloadUrl = "http://...";  // 从 JSON 解析
            
            if (info.version != currentVersion) {
                DEBUG_LOG("New version available: %s", info.version.c_str());
            }
        }
        
        return info;
    }
};
```

---

## ⚠️ 注意事项

### 1. 只支持 HTTP（不支持 HTTPS）

由于 Switch 限制，当前只支持 HTTP 协议：

```cpp
// ✅ 可以
HTTPResponse resp = net->get("http://example.com");

// ❌ 不支持（会返回错误）
HTTPResponse resp = net->get("https://example.com");
```

**解决方案**：使用支持 HTTP 的 API 端点，或者设置代理服务器。

### 2. 需要网络连接

在使用前确保 Switch 已连接到网络：

```cpp
if (!net->isInitialized()) {
    DEBUG_LOG("Network not initialized");
    return;
}

if (!net->isConnected()) {
    DEBUG_LOG("No network connection");
    return;
}
```

### 3. 同步操作会阻塞

所有网络请求都是同步的，会阻塞当前线程：

```cpp
// 这会阻塞直到请求完成或超时
HTTPResponse resp = net->get("http://slow-server.com/data");

// 建议：在后台任务或加载场景中执行
```

### 4. 超时设置

```cpp
HTTPRequest req("http://example.com");
req.timeout = 30;  // 30秒超时（默认）

// 对于慢速连接，可以增加超时
req.timeout = 60;  // 60秒
```

---

## 🐛 错误处理

### 检查错误

```cpp
HTTPResponse resp = net->get("http://example.com");

if (!resp.isSuccess()) {
    // 检查错误信息
    DEBUG_LOG("Error: %s", resp.error.c_str());
    
    // 检查状态码
    if (resp.statusCode == 0) {
        DEBUG_LOG("Connection failed");
    } else if (resp.statusCode == 404) {
        DEBUG_LOG("Not found");
    } else if (resp.statusCode >= 500) {
        DEBUG_LOG("Server error");
    }
}
```

### 常见错误

| 错误 | 原因 | 解决方案 |
|------|------|---------|
| "Network not initialized" | socket 初始化失败 | 检查 Switch 网络设置 |
| "Failed to resolve hostname" | DNS 解析失败 | 检查域名是否正确 |
| "Failed to connect" | 无法连接服务器 | 检查网络连接和 URL |
| "No response received" | 超时或连接断开 | 增加超时时间 |
| "Only HTTP is supported" | 使用了 HTTPS | 改用 HTTP |

---

## 📊 性能考虑

### 1. 请求频率

```cpp
// ❌ 不要在每帧都发送请求
void onUpdate() {
    HTTPResponse resp = net->get("http://api.example.com");  // 太频繁！
}

// ✅ 使用定时器或事件触发
float timer = 0;
void onUpdate() {
    timer += Time::deltaTime;
    if (timer >= 60.0f) {  // 每60秒一次
        HTTPResponse resp = net->get("http://api.example.com");
        timer = 0;
    }
}
```

### 2. 响应大小

```cpp
// 注意响应大小，避免内存问题
HTTPResponse resp = net->get("http://example.com/huge-file");

if (resp.body.size() > 10 * 1024 * 1024) {  // > 10MB
    DEBUG_LOG("Response too large: %zu bytes", resp.body.size());
}
```

---

## 🎯 最佳实践

1. ✅ **始终检查网络状态** - 在发送请求前检查
2. ✅ **处理错误** - 网络请求可能失败
3. ✅ **使用超时** - 设置合理的超时时间
4. ✅ **显示进度** - 对于长时间操作显示进度
5. ✅ **缓存数据** - 避免重复请求相同数据
6. ✅ **用户反馈** - 显示加载状态或错误信息

---

## 🚀 完整示例游戏

查看 `examples/network_demo.cpp` 获取完整的示例代码。

---

## 📚 相关文档

- `API_REFERENCE.md` - 完整 API 参考
- `DEBUG_CONSOLE_GUIDE.md` - 调试控制台使用
- `QUICKSTART.md` - 快速开始指南

---

**开始使用网络功能，让你的游戏连接世界！** 🌐✨
