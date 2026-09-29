#include <windows.h>
#include <winhttp.h>
#include <cctype>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <FL/Fl.H>

const unsigned long long localVersion = 2;

void cb_okcheckUPT(void*);

// WinHTTP 错误码翻译
static const char *winhttpErrText(DWORD err)
{
    switch (err) {
        case 12002: return "ERROR_WINHTTP_TIMEOUT";
        case 12007: return "ERROR_WINHTTP_NAME_NOT_RESOLVED";
        case 12029: return "ERROR_WINHTTP_CANNOT_CONNECT";
        case 12030: return "ERROR_WINHTTP_CONNECTION_ERROR";
        case 12152: return "ERROR_WINHTTP_INVALID_SERVER_RESPONSE";
        case 12175: return "ERROR_WINHTTP_SECURE_FAILURE";
        case 12181: return "ERROR_WINHTTP_INVALID_URL";
        case 87:    return "ERROR_INVALID_PARAMETER";
        default:    return "Unknown Error";
    }
}

// 单次 HTTP GET，不重试。成功返回 true，失败返回 false 并带出错误码
static bool httpGetOnce(const std::wstring& host, const std::wstring& path,
                        std::string& out, DWORD& errorCode)
{
    out.clear();
    errorCode = 0;

    HINTERNET hSession = WinHttpOpen(L"RSAPUC/1.0",
                                     WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                     WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) {
        errorCode = GetLastError();
        spdlog::warn("WinHttpOpen failed, code={} ({})", errorCode, winhttpErrText(errorCode));
        return false;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(),
                                        INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) {
        errorCode = GetLastError();
        spdlog::warn("WinHttpConnect failed, host={}, code={} ({})",
                     std::string(host.begin(), host.end()), errorCode, winhttpErrText(errorCode));
        WinHttpCloseHandle(hSession);
        return false;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
                                            nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                                            WINHTTP_FLAG_SECURE);
    if (!hRequest) {
        errorCode = GetLastError();
        spdlog::warn("WinHttpOpenRequest failed, path={}, code={} ({})",
                     std::string(path.begin(), path.end()), errorCode, winhttpErrText(errorCode));
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    // 超时设置：连接 5s，发送 5s，接收 5s
    WinHttpSetTimeouts(hRequest, 5000, 5000, 5000, 5000);

    bool ok = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                 WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
              WinHttpReceiveResponse(hRequest, nullptr);

    if (!ok) {
        errorCode = GetLastError();
        spdlog::warn("SendRequest/ReceiveResponse failed, code={} ({})",
                     errorCode, winhttpErrText(errorCode));
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    // 检查 HTTP 状态码
    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    if (WinHttpQueryHeaders(hRequest,
                            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusSize,
                            WINHTTP_NO_HEADER_INDEX)) {
        if (statusCode < 200 || statusCode >= 300) {
            spdlog::warn("HTTP status code = {}", statusCode);
        }
    }

    // 读数据
    DWORD size = 0;
    bool readOk = true;
    do {
        size = 0;
        if (!WinHttpQueryDataAvailable(hRequest, &size)) {
            errorCode = GetLastError();
            spdlog::warn("QueryDataAvailable failed, code={} ({})",
                         errorCode, winhttpErrText(errorCode));
            readOk = false;
            break;
        }
        if (!size) break;
        std::string buf(size, 0);
        DWORD read = 0;
        if (!WinHttpReadData(hRequest, &buf[0], size, &read)) {
            errorCode = GetLastError();
            spdlog::warn("ReadData failed, code={} ({})",
                         errorCode, winhttpErrText(errorCode));
            readOk = false;
            break;
        }
        out.append(buf.data(), read);
    } while (size > 0);

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return readOk;
}

// 判断是否值得重试
static bool isRetryable(DWORD err)
{
    switch (err) {
        case 12002:  // 超时
        case 12007:  // DNS 解析失败
        case 12029:  // 无法连接
        case 12030:  // 连接被中止
        case 12152:  // 服务器响应无效
            return true;
        default:
            return false;
    }
}

// 带重试的 httpGet（最多 5 次，指数退避 1s → 2s → 4s → 8s → 16s）
std::string httpGet(const std::wstring& host, const std::wstring& path,
                    DWORD& errorCode, int maxRetries = 5)
{
    std::string result;
    errorCode = 0;

    for (int attempt = 1; attempt <= maxRetries; ++attempt) {
        DWORD err = 0;
        std::string body;
        bool ok = httpGetOnce(host, path, body, err);

        // 成功且 body 非空
        if (ok && !body.empty()) {
            if (attempt > 1) {
                spdlog::info("HTTP GET successed ({})", attempt);
            }
            errorCode = 0;
            return body;
        }

        // 服务器正常返回但内容为空：不重试
        if (ok && body.empty()) {
            spdlog::warn("Server returned empty string");
            errorCode = 0;
            return body;
        }

        // 失败：判断是否值得重试
        errorCode = err;
        if (!isRetryable(err)) {
            spdlog::error("Unretryable Error! code={} ({})",
                          err, winhttpErrText(err));
            return result;
        }

        if (attempt < maxRetries) {
            int delayMs = 1000 * (1 << (attempt - 1)); // 1s,2s,4s,8s,16s
            spdlog::warn("HTTP GET FAILED {}/{}, code={} ({}), will retry after {} ms",
                         attempt, maxRetries, err, winhttpErrText(err), delayMs);
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        } else {
            spdlog::error("HTTP GET FAILED after {} attempts, code={} ({})",
                          maxRetries, err, winhttpErrText(err));
        }
    }

    return result;
}

// 去掉空白字符
std::string trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// 安全转成整数
bool toInt(const std::string& s, long long &out)
{
    if (s.empty()) return false;
    long long value = 0;
    for (char c : s) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
        value = value * 10 + (c - '0');
    }
    out = value;
    return true;
}

struct UPTInfo {
    unsigned long long localVer;
    unsigned long long remoteVer;
    bool newer = true;
    bool ok = true;
} uptinfo;

// ============================================================
// cb_bkcheckUPT：工作函数，由外部创建线程来调用，内部不再起线程
// ============================================================
void cb_bkcheckUPT()
{

    DWORD err = 0;
    std::string body = httpGet(L"updatechecker.pages.dev", L"/rsap/version.txt", err);

    if (body.empty()) {
        if (err != 0) {
            spdlog::error("HTTP GET FAILED: WinHTTP code={} ({})",
                          err, winhttpErrText(err));
        } else {
            spdlog::warn("HTTP GET returned empty body, no Error");
        }
        uptinfo.ok=false;
        Fl::awake(cb_okcheckUPT);
        return;
    }

    std::string text = trim(body);

    long long remoteBuild = 0;
    if (!toInt(text, remoteBuild)) {
        spdlog::error("Cannot Parse version text \"{}\"", text);
        Fl::awake(cb_okcheckUPT);
        return;
    }

    spdlog::debug("Local: {}", localVersion);
    spdlog::debug("Remote: {}", remoteBuild);

    uptinfo.localVer  = localVersion;
    uptinfo.remoteVer = remoteBuild;
    uptinfo.newer     = true;

    if (remoteBuild > static_cast<long long>(localVersion)) {
        spdlog::info("Local: {} < Remote: {}", localVersion, remoteBuild);
    } else {
        spdlog::info("All Up To Date");
        uptinfo.newer = false;
    }

    Fl::awake(cb_okcheckUPT);
}