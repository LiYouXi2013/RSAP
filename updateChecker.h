#include <windows.h>
#include <winhttp.h>
#include <cctype>
#include <iostream>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <FL/Fl.H>

unsigned long long localVersion = 1;

void cb_okcheckUPT(void*);

std::string httpGet(const std::wstring& host, const std::wstring& path) {
    std::string result;

    HINTERNET hSession = WinHttpOpen(L"RSAPUC/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return result;

    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(),
        INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return result; }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(),
        nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);
    if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return result; }

    if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
        WinHttpReceiveResponse(hRequest, nullptr)) {

        DWORD size = 0;
        do {
            size = 0;
            WinHttpQueryDataAvailable(hRequest, &size);
            if (!size) break;
            std::string buf(size, 0);
            DWORD read = 0;
            WinHttpReadData(hRequest, &buf[0], size, &read);
            result.append(buf.data(), read);
        } while (size > 0);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return result;
}

// 去掉空白字符
std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// 安全转成整数
bool toInt(const std::string& s, long long& out) {
    if (s.empty()) return false;

    long long value = 0;
    for (char c : s) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
        value = value * 10 + (c - '0');
    }

    out = value;
    return true;
}

struct UPTInfo{
    unsigned long long localVer;
    unsigned long long remoteVer;
    bool newer=true;
}uptinfo;

void cb_bkcheckUPT(){

    std::string body;
    body = httpGet(L"https://updatechecker.pages.dev", L"rsap/version.txt");

    std::string text = trim(body);

    long long remoteBuild = 0;
    if (!toInt(text, remoteBuild)) {
        spdlog::error("Cannot Prase version text \"{}\"",text);
    }

    spdlog::debug("Local: {}",localVersion);
    spdlog::debug("Remote: {}",remoteBuild);
    

    uptinfo.localVer=localVersion;
    uptinfo.remoteVer=remoteBuild;

    if (remoteBuild > localVersion) {
        spdlog::info("Local: {} < Remote: {}",localVersion,remoteBuild);
    } else {
        spdlog::info("All Up To Date");
        uptinfo.newer=false;
    }
    Fl::awake(cb_okcheckUPT);
}