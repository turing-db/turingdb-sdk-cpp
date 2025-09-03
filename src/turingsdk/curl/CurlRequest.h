#pragma once

#include <stdexcept>
#include <string>

#include <curl/curl.h>

#include "turingsdk/base/CallBackSignatures.h"
#include "CurlClientResult.h"

namespace turingClient {

class CurlRequest {
public:
    WriteCallBack callBackFn;

    bool init();

    CurlRequest()
        : _handle(curl_easy_init())
    {
        if (!_handle) {
            throw std::runtime_error("Failed to initialize CURL");
        }
        if (!init()) {
            throw std::runtime_error("Failed to initialize CURL");
        }
    }

    ~CurlRequest() {
        if (_handle) {
            curl_easy_cleanup(_handle);
        }
        if (_headers) {
            curl_slist_free_all(_headers);
        }
    }

    CurlRequest(CurlRequest&) = delete;
    CurlRequest& operator=(const CurlRequest&) = delete;

    CurlRequest(CurlRequest&& other) noexcept
        :callBackFn(std::move(other.callBackFn)), 
        _handle(other._handle),
        _headers(other._headers)
    {
        other._handle = nullptr;
        other._headers = nullptr;
    }

    CurlRequest& operator=(CurlRequest&& other) noexcept {
        if (this != &other) {
            if (_handle) {
                curl_easy_cleanup(_handle);
            }
            if (_headers) {
                curl_slist_free_all(_headers);
            }
            _handle = other._handle;
            _headers = other._headers;
            callBackFn = std::move(other.callBackFn);

            other._handle = nullptr;
            other._headers = nullptr;
        }
        return *this;
    };

    CurlClientResult<void> addToSlist(const std::string& header);
    void clearHeaders();
    CurlClientResult<void> clearHeader(const std::string& headerKey);
    CurlClientResult<void> fillAllHeaders();
    CurlClientResult<void> setUrl(const std::string& url);
    CurlClientResult<void> setPost(const std::string& postFields);
    CurlClientResult<void> setWriteCallBack(WriteCallBackPointer func, void* userData);
    CurlClientResult<void> setBearerToken(const std::string& bearerToken);
    CurlClientResult<void> setInstanceId(const std::string& instanceId);
    CurlClientResult<void> addHeader(const std::string& headerKey, const std::string& headerValue);
    CurlClientResult<void> send();

private:
    CURL* _handle {nullptr};
    struct curl_slist* _headers {nullptr};
    std::unordered_map<std::string, std::string> _headerMap;
};
}
