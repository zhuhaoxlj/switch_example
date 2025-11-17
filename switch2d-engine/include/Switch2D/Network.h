#pragma once
#include <switch.h>
#include <string>
#include <vector>
#include <map>
#include <functional>

namespace Switch2D {

// ============================================
// HTTPMethod - HTTP 请求方法
// ============================================
enum class HTTPMethod {
    GET,
    POST,
    PUT,
    DELETE,
    HEAD
};

// ============================================
// HTTPResponse - HTTP 响应
// ============================================
struct HTTPResponse {
    int statusCode = 0;
    std::string statusMessage;
    std::map<std::string, std::string> headers;
    std::vector<char> body;
    std::string error;
    
    bool isSuccess() const { return statusCode >= 200 && statusCode < 300; }
    std::string getBodyAsString() const { return std::string(body.begin(), body.end()); }
};

// ============================================
// HTTPRequest - HTTP 请求配置
// ============================================
struct HTTPRequest {
    std::string url;
    HTTPMethod method = HTTPMethod::GET;
    std::map<std::string, std::string> headers;
    std::string body;
    int timeout = 30;  // 秒
    
    HTTPRequest(const std::string& url) : url(url) {}
};

// ============================================
// NetworkManager - 网络管理器
// ============================================
class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();
    
    // 初始化和清理
    bool initialize();
    void shutdown();
    bool isInitialized() const { return initialized; }
    
    // HTTP 请求（同步）
    HTTPResponse request(const HTTPRequest& req);
    HTTPResponse get(const std::string& url);
    HTTPResponse post(const std::string& url, const std::string& body, 
                      const std::string& contentType = "application/json");
    
    // 下载文件
    bool downloadFile(const std::string& url, const std::string& savePath,
                     std::function<void(size_t, size_t)> progressCallback = nullptr);
    
    // DNS 解析
    std::string resolveHost(const std::string& hostname);
    
    // 工具函数
    std::string urlEncode(const std::string& str);
    std::string urlDecode(const std::string& str);
    
    // 设置默认请求头
    void setDefaultHeader(const std::string& key, const std::string& value);
    void removeDefaultHeader(const std::string& key);
    
    // 获取网络状态
    bool isConnected();
    std::string getLocalIP();
    
private:
    bool initialized = false;
    std::map<std::string, std::string> defaultHeaders;
    
    // 内部辅助函数
    HTTPResponse performRequest(const HTTPRequest& req);
    bool parseURL(const std::string& url, std::string& protocol, 
                  std::string& host, int& port, std::string& path);
    std::string buildHTTPRequest(const HTTPRequest& req, const std::string& host);
    HTTPResponse parseHTTPResponse(const std::vector<char>& data);
    
    // Socket 操作
    int createSocket();
    bool connectSocket(int sock, const std::string& host, int port);
    bool sendData(int sock, const char* data, size_t len);
    std::vector<char> receiveData(int sock, int timeout);
};

} // namespace Switch2D
