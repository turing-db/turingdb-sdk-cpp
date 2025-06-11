#include "CurlRequest.h"

#include <curl/curl.h>

#include "Profiler.h"

using namespace turingClient;

static size_t staticCallBack(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* self = static_cast<CurlRequest*>(userdata);
    return self->callBackFn(ptr, size, nmemb, userdata);
}

CurlClientResult<void> CurlRequest::setUrl(const std::string& url) {
    Profile profile {"CurlRequest::setUrl"};

    if (auto res = curl_easy_setopt(_handle, CURLOPT_URL, url.c_str()); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }
    return {};
}

CurlClientResult<void> CurlRequest::setPost(const std::string& postFields) {
    Profile profile {"CurlRequest::setPost"};

    if (auto res = curl_easy_setopt(_handle, CURLOPT_POST, 1L); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_TCP_NODELAY, 1L); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_HTTP_TRANSFER_DECODING, 1L); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_HTTP_CONTENT_DECODING, 1L); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_ACCEPT_ENCODING, ""); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_POSTFIELDS, postFields.c_str()); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_POSTFIELDSIZE, postFields.size()); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_FAILONERROR, 1L); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_BUFFERSIZE, CURL_MAX_READ_SIZE); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }

    return {};
}

CurlClientResult<void> CurlRequest::setWriteCallBack(WriteCallBack& func) {
    Profile profile {"CurlRequest::setWriteCallBack"};

    callBackFn = std::move(func);
    if (auto res = curl_easy_setopt(_handle, CURLOPT_WRITEFUNCTION, staticCallBack); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_WRITE_CALLBACK, res);
    }
    if (auto res = curl_easy_setopt(_handle, CURLOPT_WRITEDATA, this); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_WRITE_CALLBACK, res);
    }

    return {};
}

CurlClientResult<void> CurlRequest::send() {
    Profile profile {"CurlRequest::send"};

    if (auto res = curl_easy_perform(_handle); res != CURLE_OK) {
        if (res == CURLE_HTTP_RETURNED_ERROR) {
            long responseCode {};
            curl_easy_getinfo(_handle, CURLINFO_RESPONSE_CODE, &responseCode);
            return CurlClientError::result(CurlClientErrorType::HTTP_ERROR, responseCode);
        }


        return CurlClientError::result(CurlClientErrorType::CANNOT_SEND_REQUEST, res);
    }

    return {};
}
