# 🌐 X 按键网络测试功能

## ✨ 新增功能

已在游戏中添加**按 X 键请求百度首页**的功能！

---

## 🎮 如何使用

### 1. 运行游戏

在 Ryujinx 中加载 `switch2d-demo.nro`

### 2. 确保网络连接

确保 Switch（或模拟器）已连接到互联网

### 3. 按 X 键

在游戏中按下 **X 按钮**（键盘上的 C 键）

### 4. 查看结果

**按 A+B** 打开调试控制台，查看百度首页的源代码！

---

## 📊 显示内容

当你按下 X 键后，调试控制台会显示：

```
=== Fetching Baidu ===
URL: http://www.baidu.com
Please wait...
SUCCESS! Status: 200
Content-Type: text/html
Server: BWS/1.1
Page size: 2443 bytes
Preview (first 500 chars):
<!DOCTYPE html><!--STATUS OK-->
<html>
<head>
    <meta http-equiv="content-type" content="text/html;charset=utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=Edge">
    ...
Page title: 百度一下，你就知道
Request complete!
```

---

## 🔧 功能详情

### 实现的功能

1. ✅ **HTTP GET 请求** - 向 www.baidu.com 发送请求
2. ✅ **响应解析** - 解析 HTTP 响应头和状态码
3. ✅ **内容显示** - 显示HTML源码（前500字符）
4. ✅ **标题提取** - 自动提取网页标题
5. ✅ **错误处理** - 处理网络错误和超时

### 显示信息

- ✅ HTTP 状态码（200, 404, 等）
- ✅ Content-Type 响应头
- ✅ Server 信息
- ✅ 页面大小（字节数）
- ✅ HTML 内容预览
- ✅ 网页标题

---

## 💻 实现代码

### 关键代码片段

```cpp
// 在 DemoScene 中添加
void onUpdate() override {
    // X 键 - 请求百度网页源码
    if (input->getButtonDown(Button::X)) {
        fetchBaiduPage();
    }
}

void fetchBaiduPage() {
    NetworkManager* net = Engine::getInstance().getNetwork();
    
    if (!net->isInitialized()) {
        DEBUG_LOG("ERROR: Network not initialized!");
        return;
    }
    
    // 发送 GET 请求
    HTTPResponse resp = net->get("http://www.baidu.com");
    
    if (resp.isSuccess()) {
        std::string html = resp.getBodyAsString();
        DEBUG_LOG("Page size: %zu bytes", html.length());
        DEBUG_LOG("Preview: %s", html.substr(0, 500).c_str());
        
        // 提取标题
        size_t titleStart = html.find("<title>");
        if (titleStart != std::string::npos) {
            size_t titleEnd = html.find("</title>", titleStart);
            std::string title = html.substr(titleStart + 7, titleEnd - titleStart - 7);
            DEBUG_LOG("Title: %s", title.c_str());
        }
    }
}
```

---

## 🎯 测试步骤

### 步骤 1：启动游戏
```
在 Ryujinx 中加载游戏
```

### 步骤 2：打开调试控制台
```
按 A + B 键
```

### 步骤 3：测试网络
```
按 X 键（或键盘 C）
```

### 步骤 4：查看结果
```
在调试控制台中查看网页源码
```

---

## 🎮 完整控制说明

| 按键 | 功能 |
|------|------|
| D-Pad/左摇杆 | 移动玩家 |
| A 或 B | 跳跃 |
| **X** | **请求百度首页（网络测试）** ← NEW! |
| A + B 同时 | 切换调试控制台 |
| + 键 | 退出游戏 |

---

## 📝 示例输出

### 成功情况

```
=== Fetching Baidu ===
URL: http://www.baidu.com
Please wait...
SUCCESS! Status: 200
Content-Type: text/html; charset=utf-8
Server: BWS/1.1
Page size: 2443 bytes
Preview (first 500 chars):
<!DOCTYPE html>
<!--STATUS OK-->
<html>
<head>
    <meta http-equiv="content-type" content="text/html;charset=utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=Edge">
    <meta http-equiv="Cache-Control" content="no-siteapp" />
    <title>百度一下，你就知道</title>
    ...
... (truncated)
Page title: 百度一下，你就知道
Request complete!
```

### 失败情况（无网络）

```
=== Fetching Baidu ===
URL: http://www.baidu.com
Please wait...
FAILED!
Error: Failed to connect to www.baidu.com
Make sure Switch is connected to network
```

---

## 🔍 技术细节

### 使用的 API

```cpp
// 获取网络管理器
NetworkManager* net = Engine::getInstance().getNetwork();

// 发送 HTTP GET 请求
HTTPResponse resp = net->get("http://www.baidu.com");

// 检查是否成功
if (resp.isSuccess()) {
    // statusCode 在 200-299 范围内
}

// 获取响应体为字符串
std::string html = resp.getBodyAsString();

// 访问响应头
std::string contentType = resp.headers["Content-Type"];

// 访问状态码和消息
int code = resp.statusCode;
std::string message = resp.statusMessage;
```

### 特殊处理

1. **内容截断** - 只显示前 500 个字符（避免控制台溢出）
2. **标题提取** - 使用字符串查找提取 `<title>` 标签内容
3. **错误处理** - 检查网络初始化和请求结果
4. **中文支持** - 正确显示中文字符（UTF-8）

---

## 🌟 扩展建议

### 可以尝试的改进

1. **请求其他网站**
   ```cpp
   resp = net->get("http://example.com");
   resp = net->get("http://httpbin.org/ip");
   ```

2. **添加更多按键**
   ```cpp
   if (input->getButtonDown(Button::Y)) {
       fetchGitHubAPI();
   }
   ```

3. **保存到文件**
   ```cpp
   FILE* f = fopen("baidu.html", "w");
   fwrite(html.c_str(), 1, html.length(), f);
   fclose(f);
   ```

4. **解析 JSON**
   ```cpp
   resp = net->get("http://api.github.com");
   std::string json = resp.getBodyAsString();
   // 解析 JSON...
   ```

---

## ⚠️ 注意事项

1. **需要网络连接** - 确保 Switch 已连接 WiFi
2. **HTTP 协议** - 只支持 HTTP，不支持 HTTPS
3. **阻塞操作** - 请求会暂停游戏直到完成（约 1-3 秒）
4. **调试控制台** - 必须打开才能看到结果
5. **重复按键** - 可以多次按 X 键重复请求

---

## 🐛 故障排除

### 问题：按 X 键无反应

**解决方案**：
1. 检查是否已打开调试控制台（A+B）
2. 确认网络已初始化（启动日志中应有相关信息）

### 问题：显示 "Network not initialized"

**解决方案**：
1. 确保 Switch 连接到网络
2. 检查 Ryujinx 网络设置
3. 查看启动日志中的网络初始化信息

### 问题：请求超时

**解决方案**：
1. 检查网络连接
2. 尝试其他网站（如 http://example.com）
3. 等待更长时间（默认超时 30 秒）

---

## 📚 相关文档

- [NETWORK_GUIDE.md](NETWORK_GUIDE.md) - 完整网络模块使用指南
- [NETWORK_QUICK_REF.md](NETWORK_QUICK_REF.md) - 网络 API 快速参考
- [DEBUG_CONSOLE_GUIDE.md](DEBUG_CONSOLE_GUIDE.md) - 调试控制台使用

---

**现在就试试按 X 键获取百度首页吧！** 🚀🌐

---

*功能已完成 - 编译成功 - 随时可用*
