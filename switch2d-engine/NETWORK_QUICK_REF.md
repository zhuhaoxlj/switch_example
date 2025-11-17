# 🌐 网络模块快速参考

Switch2D Engine - Network Module Quick Reference

---

## ⚡ 快速开始

```cpp
#include "Switch2D/Switch2D.h"
using namespace Switch2D;

// 获取网络管理器
NetworkManager* net = Engine::getInstance().getNetwork();

// 发送 GET 请求
HTTPResponse resp = net->get("http://example.com");

if (resp.isSuccess()) {
    std::string html = resp.getBodyAsString();
    DEBUG_LOG("Success: %s", html.c_str());
}
```

---

## 📚 API 速查表

### HTTP 请求

| 方法 | 用途 | 示例 |
|------|------|------|
| `get(url)` | GET 请求 | `net->get("http://api.com/data")` |
| `post(url, body, type)` | POST 请求 | `net->post(url, json, "application/json")` |
| `request(req)` | 自定义请求 | `net->request(customRequest)` |

### 文件操作

| 方法 | 用途 | 示例 |
|------|------|------|
| `downloadFile(url, path)` | 下载文件 | `net->downloadFile(url, "file.zip")` |
| `downloadFile(url, path, callback)` | 带进度下载 | `net->downloadFile(url, path, progress)` |

### 工具函数

| 方法 | 用途 | 示例 |
|------|------|------|
| `resolveHost(hostname)` | DNS 解析 | `net->resolveHost("example.com")` |
| `urlEncode(str)` | URL 编码 | `net->urlEncode("Hello World")` |
| `urlDecode(str)` | URL 解码 | `net->urlDecode("Hello%20World")` |
| `isConnected()` | 检查网络 | `if (net->isConnected()) { ... }` |
| `isInitialized()` | 检查初始化 | `if (net->isInitialized()) { ... }` |

### 配置

| 方法 | 用途 | 示例 |
|------|------|------|
| `setDefaultHeader(key, val)` | 设置默认头 | `net->setDefaultHeader("User-Agent", "MyApp")` |
| `removeDefaultHeader(key)` | 移除默认头 | `net->removeDefaultHeader("User-Agent")` |

---

## 🔧 HTTPRequest 结构

```cpp
HTTPRequest req("http://example.com/api");
req.method = HTTPMethod::POST;          // GET, POST, PUT, DELETE, HEAD
req.headers["Authorization"] = "Bearer TOKEN";
req.headers["Content-Type"] = "application/json";
req.body = R"({"key": "value"})";
req.timeout = 30;                       // 秒
```

---

## 📊 HTTPResponse 结构

```cpp
HTTPResponse resp = net->get(url);

// 状态
resp.statusCode;          // 200, 404, 500, etc.
resp.statusMessage;       // "OK", "Not Found", etc.
resp.error;               // 错误信息（如有）

// 响应头
resp.headers["Content-Type"];
resp.headers["Content-Length"];

// 响应体
resp.body;                // vector<char>
resp.getBodyAsString();   // 转为 string

// 便捷方法
resp.isSuccess();         // statusCode 200-299
```

---

## 💡 常用代码片段

### 1. 基本 GET 请求

```cpp
HTTPResponse resp = net->get("http://api.example.com/data");
if (resp.isSuccess()) {
    DEBUG_LOG("%s", resp.getBodyAsString().c_str());
}
```

### 2. POST JSON 数据

```cpp
std::string json = R"({"name":"Player","score":100})";
HTTPResponse resp = net->post(
    "http://api.example.com/submit",
    json,
    "application/json"
);
```

### 3. 自定义请求头

```cpp
HTTPRequest req("http://api.example.com/secure");
req.headers["Authorization"] = "Bearer YOUR_TOKEN";
req.headers["X-API-Key"] = "YOUR_API_KEY";
HTTPResponse resp = net->request(req);
```

### 4. 下载进度

```cpp
net->downloadFile(url, path, 
    [](size_t current, size_t total) {
        float percent = current * 100.0f / total;
        DEBUG_LOG("%.1f%%", percent);
    }
);
```

### 5. 错误处理

```cpp
HTTPResponse resp = net->get(url);
if (!resp.isSuccess()) {
    if (!resp.error.empty()) {
        DEBUG_LOG("Error: %s", resp.error.c_str());
    } else {
        DEBUG_LOG("HTTP %d: %s", 
                 resp.statusCode, 
                 resp.statusMessage.c_str());
    }
}
```

### 6. 带参数的 GET 请求

```cpp
std::string query = "?name=" + net->urlEncode("张三") +
                   "&age=25" +
                   "&city=" + net->urlEncode("北京");
HTTPResponse resp = net->get("http://api.example.com/search" + query);
```

### 7. 检查网络可用性

```cpp
if (!net->isInitialized()) {
    DEBUG_LOG("Network not available");
    return;
}

if (!net->isConnected()) {
    DEBUG_LOG("No network connection");
    return;
}

// 安全发送请求
HTTPResponse resp = net->get(url);
```

### 8. 解析 HTML

```cpp
HTTPResponse resp = net->get("http://example.com");
if (resp.isSuccess()) {
    std::string html = resp.getBodyAsString();
    
    // 查找特定内容
    size_t pos = html.find("<title>");
    if (pos != std::string::npos) {
        size_t end = html.find("</title>", pos);
        std::string title = html.substr(pos + 7, end - pos - 7);
        DEBUG_LOG("Title: %s", title.c_str());
    }
}
```

---

## ⚠️ 重要提示

### ✅ 支持
- HTTP (端口 80)
- GET, POST, PUT, DELETE, HEAD
- 自定义请求头
- DNS 解析
- 文件下载

### ❌ 不支持
- HTTPS（SSL/TLS）
- WebSocket
- FTP
- 异步/非阻塞请求

### 💡 最佳实践
1. 始终检查 `isInitialized()`
2. 处理所有错误情况
3. 使用合理的超时时间
4. 不要在每帧发送请求
5. 大文件下载使用进度回调

---

## 🐛 常见问题

| 问题 | 解决方案 |
|------|----------|
| "Network not initialized" | 检查 Switch 网络设置 |
| "Failed to resolve hostname" | 检查 DNS 或使用 IP 地址 |
| "Only HTTP is supported" | 不要使用 `https://` |
| 请求超时 | 增加 `req.timeout` 值 |
| 响应为空 | 检查 URL 和网络连接 |

---

## 📚 完整文档

- `NETWORK_GUIDE.md` - 完整使用指南
- `examples/network_example.cpp` - 示例代码
- `API_REFERENCE.md` - API 详细参考

---

**快速开始网络编程！** 🚀
