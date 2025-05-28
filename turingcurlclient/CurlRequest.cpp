#include "CurlRequest.h"
#include <curl/curl.h>
#include <iostream>

using namespace turingClient;

static size_t staticCallBack(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* self = static_cast<CurlRequest*>(userdata);
    return self->callBackFn(ptr, size, nmemb, userdata);
}

int CurlRequest::setUrl(std::string& url) {
    if (auto res = curl_easy_setopt(_handle, CURLOPT_URL, url.c_str()); res != CURLE_OK) {
        return 1;
    }
    return 0;
}

int CurlRequest::setPost(std::string& postFields) {
    if (auto res = curl_easy_setopt(_handle, CURLOPT_POST, 1L); res != CURLE_OK) {
        return 1;
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_POSTFIELDS, postFields.c_str()); res != CURLE_OK) {
        return 1;
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_POSTFIELDSIZE, postFields.size()); res != CURLE_OK) {
        return 1;
    }

    return 0;
}

int CurlRequest::setWriteCallBack(WriteCallBack func) {
    callBackFn = std::move(func);
    if (auto res = curl_easy_setopt(_handle, CURLOPT_WRITEFUNCTION, staticCallBack); res != CURLE_OK) {
        return 1;
    }
    if (auto res = curl_easy_setopt(_handle, CURLOPT_WRITEDATA, this); res != CURLE_OK) {
        return 1;
    }

    return 0;
}
int CurlRequest::send() {
    if (auto res = curl_easy_perform(_handle); res != CURLE_OK) {
        std::cerr << "Failed to perform: " << curl_easy_strerror(res) << std::endl;
        return 1;
    }

    return 0;
}
