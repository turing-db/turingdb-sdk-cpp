#pragma once

#include <curl/curl.h>
#include <stdexcept>
#include <string>

#include "CallBackSignatures.h"


namespace turingClient {

class CurlRequest {
public:
    WriteCallBack callBackFn;

    CurlRequest()
        : _handle(curl_easy_init()) {
        if (!_handle) {
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
        : _handle(other._handle) {
        other._handle = nullptr;
    }

    CurlRequest& operator=(CurlRequest&& other) noexcept {
        if (this != &other) {
            if (_handle) {
                curl_easy_cleanup(_handle);
            }
            _handle = other._handle;
            other._handle = nullptr;
        }
        return *this;
    };

    int setUrl(std::string& url);
    int setPost(std::string& postFields);
    int setWriteCallBack(WriteCallBack func);
    int send();

private:
    CURL* _handle;
};

}
