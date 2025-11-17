#include "Switch2D/Network.h"
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

namespace Switch2D {

// ============================================
// NetworkManager 实现
// ============================================

NetworkManager::NetworkManager() {
    // 设置默认请求头
    defaultHeaders["User-Agent"] = "Switch2D-Engine/1.0";
    defaultHeaders["Accept"] = "*/*";
}

NetworkManager::~NetworkManager() {
    shutdown();
}

bool NetworkManager::initialize() {
    if (initialized) {
        return true;
    }
    
    // 初始化 socket 服务
    Result rc = socketInitializeDefault();
    if (R_FAILED(rc)) {
        printf("socketInitializeDefault failed: 0x%x\n", rc);
        return false;
    }
    
    initialized = true;
    printf("Network initialized successfully\n");
    return true;
}

void NetworkManager::shutdown() {
    if (!initialized) {
        return;
    }
    
    socketExit();
    initialized = false;
    printf("Network shutdown\n");
}

// ============================================
// HTTP 请求
// ============================================

HTTPResponse NetworkManager::get(const std::string& url) {
    HTTPRequest req(url);
    req.method = HTTPMethod::GET;
    return request(req);
}

HTTPResponse NetworkManager::post(const std::string& url, const std::string& body, 
                                  const std::string& contentType) {
    HTTPRequest req(url);
    req.method = HTTPMethod::POST;
    req.body = body;
    req.headers["Content-Type"] = contentType;
    req.headers["Content-Length"] = std::to_string(body.length());
    return request(req);
}

HTTPResponse NetworkManager::request(const HTTPRequest& req) {
    if (!initialized) {
        HTTPResponse resp;
        resp.error = "Network not initialized";
        return resp;
    }
    
    return performRequest(req);
}

// ============================================
// HTTP 请求实现
// ============================================

HTTPResponse NetworkManager::performRequest(const HTTPRequest& req) {
    HTTPResponse response;
    
    // 解析 URL
    std::string protocol, host, path;
    int port;
    if (!parseURL(req.url, protocol, host, port, path)) {
        response.error = "Invalid URL";
        return response;
    }
    
    // 只支持 HTTP
    if (protocol != "http") {
        response.error = "Only HTTP is supported (not HTTPS)";
        return response;
    }
    
    // 解析主机名
    std::string ip = resolveHost(host);
    if (ip.empty()) {
        response.error = "Failed to resolve hostname: " + host;
        return response;
    }
    
    printf("Connecting to %s (%s):%d\n", host.c_str(), ip.c_str(), port);
    
    // 创建 socket
    int sock = createSocket();
    if (sock < 0) {
        response.error = "Failed to create socket";
        return response;
    }
    
    // 连接
    if (!connectSocket(sock, ip, port)) {
        response.error = "Failed to connect to " + host;
        close(sock);
        return response;
    }
    
    // 构建 HTTP 请求
    std::string httpReq = buildHTTPRequest(req, host);
    
    // 发送请求
    if (!sendData(sock, httpReq.c_str(), httpReq.length())) {
        response.error = "Failed to send request";
        close(sock);
        return response;
    }
    
    // 接收响应
    std::vector<char> data = receiveData(sock, req.timeout);
    close(sock);
    
    if (data.empty()) {
        response.error = "No response received";
        return response;
    }
    
    // 解析响应
    return parseHTTPResponse(data);
}

// ============================================
// URL 解析
// ============================================

bool NetworkManager::parseURL(const std::string& url, std::string& protocol, 
                              std::string& host, int& port, std::string& path) {
    // 简单的 URL 解析
    size_t pos = url.find("://");
    if (pos == std::string::npos) {
        return false;
    }
    
    protocol = url.substr(0, pos);
    std::transform(protocol.begin(), protocol.end(), protocol.begin(), ::tolower);
    
    size_t start = pos + 3;
    size_t pathPos = url.find('/', start);
    
    std::string hostPort;
    if (pathPos != std::string::npos) {
        hostPort = url.substr(start, pathPos - start);
        path = url.substr(pathPos);
    } else {
        hostPort = url.substr(start);
        path = "/";
    }
    
    // 解析端口
    size_t portPos = hostPort.find(':');
    if (portPos != std::string::npos) {
        host = hostPort.substr(0, portPos);
        port = std::stoi(hostPort.substr(portPos + 1));
    } else {
        host = hostPort;
        port = (protocol == "https") ? 443 : 80;
    }
    
    return true;
}

// ============================================
// 构建 HTTP 请求
// ============================================

std::string NetworkManager::buildHTTPRequest(const HTTPRequest& req, const std::string& host) {
    std::ostringstream oss;
    
    // 请求行
    const char* methodStr = "GET";
    switch (req.method) {
        case HTTPMethod::GET: methodStr = "GET"; break;
        case HTTPMethod::POST: methodStr = "POST"; break;
        case HTTPMethod::PUT: methodStr = "PUT"; break;
        case HTTPMethod::DELETE: methodStr = "DELETE"; break;
        case HTTPMethod::HEAD: methodStr = "HEAD"; break;
    }
    
    std::string protocol, hostTemp, path;
    int port;
    parseURL(req.url, protocol, hostTemp, port, path);
    
    oss << methodStr << " " << path << " HTTP/1.1\r\n";
    oss << "Host: " << host << "\r\n";
    
    // 默认请求头
    for (const auto& header : defaultHeaders) {
        if (req.headers.find(header.first) == req.headers.end()) {
            oss << header.first << ": " << header.second << "\r\n";
        }
    }
    
    // 自定义请求头
    for (const auto& header : req.headers) {
        oss << header.first << ": " << header.second << "\r\n";
    }
    
    oss << "Connection: close\r\n";
    oss << "\r\n";
    
    // 请求体
    if (!req.body.empty()) {
        oss << req.body;
    }
    
    return oss.str();
}

// ============================================
// 解析 HTTP 响应
// ============================================

HTTPResponse NetworkManager::parseHTTPResponse(const std::vector<char>& data) {
    HTTPResponse response;
    
    // 查找头部和主体的分隔符
    std::string dataStr(data.begin(), data.end());
    size_t headerEnd = dataStr.find("\r\n\r\n");
    
    if (headerEnd == std::string::npos) {
        response.error = "Invalid HTTP response";
        return response;
    }
    
    std::string headerStr = dataStr.substr(0, headerEnd);
    std::string bodyStr = dataStr.substr(headerEnd + 4);
    
    // 解析状态行
    std::istringstream iss(headerStr);
    std::string line;
    
    if (std::getline(iss, line)) {
        // HTTP/1.1 200 OK
        std::istringstream lineStream(line);
        std::string httpVersion;
        lineStream >> httpVersion >> response.statusCode;
        std::getline(lineStream, response.statusMessage);
        
        // 去除前导空格
        size_t start = response.statusMessage.find_first_not_of(" \r\n");
        if (start != std::string::npos) {
            response.statusMessage = response.statusMessage.substr(start);
        }
    }
    
    // 解析响应头
    while (std::getline(iss, line) && !line.empty() && line != "\r") {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // 去除空格
            size_t start = value.find_first_not_of(" \r\n");
            if (start != std::string::npos) {
                value = value.substr(start);
            }
            size_t end = value.find_last_not_of(" \r\n");
            if (end != std::string::npos) {
                value = value.substr(0, end + 1);
            }
            
            response.headers[key] = value;
        }
    }
    
    // 主体
    response.body = std::vector<char>(bodyStr.begin(), bodyStr.end());
    
    return response;
}

// ============================================
// Socket 操作
// ============================================

int NetworkManager::createSocket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("socket() failed: %d\n", errno);
    }
    return sock;
}

bool NetworkManager::connectSocket(int sock, const std::string& host, int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
        printf("inet_pton() failed\n");
        return false;
    }
    
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        printf("connect() failed: %d\n", errno);
        return false;
    }
    
    return true;
}

bool NetworkManager::sendData(int sock, const char* data, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(sock, data + sent, len - sent, 0);
        if (n <= 0) {
            printf("send() failed: %d\n", errno);
            return false;
        }
        sent += n;
    }
    return true;
}

std::vector<char> NetworkManager::receiveData(int sock, int timeout) {
    std::vector<char> buffer;
    char temp[4096];
    
    // 设置超时
    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
    
    while (true) {
        ssize_t n = recv(sock, temp, sizeof(temp), 0);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  // 超时
            }
            printf("recv() failed: %d\n", errno);
            break;
        } else if (n == 0) {
            break;  // 连接关闭
        }
        
        buffer.insert(buffer.end(), temp, temp + n);
    }
    
    return buffer;
}

// ============================================
// DNS 解析
// ============================================

std::string NetworkManager::resolveHost(const std::string& hostname) {
    // 检查是否已经是 IP 地址
    struct in_addr addr;
    if (inet_pton(AF_INET, hostname.c_str(), &addr) == 1) {
        return hostname;
    }
    
    // DNS 解析
    struct hostent* he = gethostbyname(hostname.c_str());
    if (he == nullptr) {
        printf("gethostbyname(%s) failed: %d\n", hostname.c_str(), h_errno);
        return "";
    }
    
    struct in_addr** addr_list = (struct in_addr**)he->h_addr_list;
    if (addr_list[0] != nullptr) {
        return inet_ntoa(*addr_list[0]);
    }
    
    return "";
}

// ============================================
// 下载文件
// ============================================

bool NetworkManager::downloadFile(const std::string& url, const std::string& savePath,
                                  std::function<void(size_t, size_t)> progressCallback) {
    HTTPResponse resp = get(url);
    
    if (!resp.isSuccess()) {
        printf("Download failed: %s\n", resp.error.c_str());
        return false;
    }
    
    FILE* f = fopen(savePath.c_str(), "wb");
    if (!f) {
        printf("Failed to open file: %s\n", savePath.c_str());
        return false;
    }
    
    fwrite(resp.body.data(), 1, resp.body.size(), f);
    fclose(f);
    
    if (progressCallback) {
        progressCallback(resp.body.size(), resp.body.size());
    }
    
    return true;
}

// ============================================
// URL 编码/解码
// ============================================

std::string NetworkManager::urlEncode(const std::string& str) {
    std::ostringstream oss;
    oss << std::hex;
    
    for (unsigned char c : str) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            oss << c;
        } else {
            oss << '%' << std::setw(2) << std::setfill('0') << (int)c;
        }
    }
    
    return oss.str();
}

std::string NetworkManager::urlDecode(const std::string& str) {
    std::string result;
    
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '%' && i + 2 < str.length()) {
            int value;
            std::istringstream iss(str.substr(i + 1, 2));
            if (iss >> std::hex >> value) {
                result += static_cast<char>(value);
                i += 2;
            } else {
                result += str[i];
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    
    return result;
}

// ============================================
// 网络状态
// ============================================

bool NetworkManager::isConnected() {
    if (!initialized) {
        return false;
    }
    
    // 简单测试：尝试创建 socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return false;
    }
    close(sock);
    return true;
}

std::string NetworkManager::getLocalIP() {
    // 这是一个简化实现
    // 实际上需要枚举网络接口来获取正确的 IP
    return "0.0.0.0";
}

// ============================================
// 默认请求头管理
// ============================================

void NetworkManager::setDefaultHeader(const std::string& key, const std::string& value) {
    defaultHeaders[key] = value;
}

void NetworkManager::removeDefaultHeader(const std::string& key) {
    defaultHeaders.erase(key);
}

} // namespace Switch2D
