#pragma once

#include <curl/curl.h>
#include <stdexcept>
#include <string>

#include "CallBackSignatures.h"
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
    }

    CurlRequest(CurlRequest&) = delete;
    CurlRequest& operator=(const CurlRequest&) = delete;

    CurlRequest(CurlRequest&& other) noexcept
        :callBackFn(std::move(other.callBackFn)), 
        _handle(other._handle) {
        other._handle = nullptr;
    }

    CurlRequest& operator=(CurlRequest&& other) noexcept {
        if (this != &other) {
            if (_handle) {
                curl_easy_cleanup(_handle);
            }
            _handle = other._handle;
            callBackFn = std::move(other.callBackFn);
            other._handle = nullptr;
        }
        return *this;
    };

    CurlClientResult<void> setUrl(const std::string& url);
    CurlClientResult<void> setPost(const std::string& postFields);
    CurlClientResult<void> setWriteCallBack(WriteCallBack& func);
    CurlClientResult<void> send();

private:
    CURL* _handle;
};

}
