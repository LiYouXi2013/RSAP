/*
RSAP
Copyright (C) 2026 LiYouXi2013, Candyman_RDFZ

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once
#include <windows.h>
#include <winhttp.h>
#include <cctype>
#include <string>
#include <thread>
#include <chrono>
#include <spdlog/spdlog.h>
#include <FL/Fl.H>

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
    DWORD statusCode = 0;
    DWORD statusSize = sizeof(statusCode);
    WinHttpQueryHeaders(hRequest,
                        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                        WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusSize,
                        WINHTTP_NO_HEADER_INDEX);

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

static bool isRetryable(DWORD err)
{
    switch (err) {
        case 12002:
        case 12007:
        case 12029:
        case 12030:
        case 12152:
            return true;
        default:
            return false;
    }
}

static std::string httpGet(const std::wstring& host, const std::wstring& path,
                           DWORD& errorCode, int maxRetries = 5)
{
    std::string result;
    errorCode = 0;
    for (int attempt = 1; attempt <= maxRetries; ++attempt) {
        DWORD err = 0;
        std::string body;
        bool ok = httpGetOnce(host, path, body, err);
        if (ok && !body.empty()) {
            if (attempt > 1) spdlog::info("HTTP GET successed ({})", attempt);
            errorCode = 0;
            return body;
        }
        if (ok && body.empty()) {
            spdlog::warn("Server returned empty string");
            errorCode = 0;
            return body;
        }
        errorCode = err;
        if (!isRetryable(err)) {
            spdlog::error("Unretryable Error! code={} ({})", err, winhttpErrText(err));
            return result;
        }
        if (attempt < maxRetries) {
            int delayMs = 1000 * (1 << (attempt - 1));
            spdlog::warn("HTTP GET FAILED {}/{}, code={} ({}), will retry after {} ms",
                         attempt, maxRetries, err, winhttpErrText(err), delayMs);
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
    }
    return result;
}

static std::string trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

static bool toInt(const std::string& s, long long &out)
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

class UpdateChecker
{
public:
    struct UPTInfo {
        unsigned long long localVer{4};
        unsigned long long remoteVer{0};
        bool newer = true;
        bool ok = true;
    };
    UPTInfo info;
    bool doing = false;

    static void awakeCb(void* ud);
    void backgroundCheck();
private:
    void workThread();
};
