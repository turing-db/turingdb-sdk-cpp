#include "CurlRequest.h"

#include <curl/curl.h>
#include <iostream>

#include "Profiler.h"

using namespace turingClient;

bool CurlRequest::init() {
    if (auto res = curl_easy_setopt(_handle, CURLOPT_TCP_NODELAY, 1L); res != CURLE_OK) {
        return false;
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_HTTP_TRANSFER_DECODING, 1L); res != CURLE_OK) {
        return false;
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_HTTP_CONTENT_DECODING, 1L); res != CURLE_OK) {
        return false;
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_ACCEPT_ENCODING, ""); res != CURLE_OK) {
        return false;
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1); res != CURLE_OK) {
        return false;
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_FAILONERROR, 1L); res != CURLE_OK) {
        return false;
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_BUFFERSIZE, CURL_MAX_READ_SIZE); res != CURLE_OK) {
        return false;
    }

    return true;
}

CurlClientResult<void> CurlRequest::addToSlist(const std::string& header) {
    Profile profile {"CurlRequest::addHeader"};

    struct curl_slist* temp = curl_slist_append(_headers, header.c_str());

    if (!temp) {
        curl_slist_free_all(_headers);
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_HEADERS);
    }

    _headers = temp;
    return {};
}

void CurlRequest::clearHeaders() {
    Profile profile {"CurlRequest::clearHeaders"};
    curl_slist_free_all(_headers);
    _headers = NULL;
}

CurlClientResult<void> CurlRequest::clearHeader(const std::string& headerKey) {
    Profile profile {"CurlRequest::clearHeader"};
    _headerMap.erase(headerKey);
    return fillAllHeaders();
}

CurlClientResult<void> CurlRequest::fillAllHeaders() {
    Profile profile {"CurlRequest::fillAllHeaders"};

    clearHeaders();
    for (const auto& [key, value] : _headerMap) {
        if (auto res = addToSlist(key + ": " + value); !res) {
            return res;
        }
    }

    curl_easy_setopt(_handle, CURLOPT_HTTPHEADER, _headers);
    return {};
}

CurlClientResult<void> CurlRequest::setBearerToken(const std::string& bearerToken) {
    Profile profile {"CurlRequest::setBearerToken"};

    if (_headerMap.find("Authorization") != _headerMap.end()) {
        _headerMap["Authorization"] = "Bearer " + bearerToken;
        return fillAllHeaders();
    }

    _headerMap["Authorization"] = "Bearer " + bearerToken;

    if (auto res = addToSlist("Authorization: " + _headerMap["Authorization"]); !res) {
        return res;
    }

    curl_easy_setopt(_handle, CURLOPT_HTTPHEADER, _headers);
    return {};
}

CurlClientResult<void> CurlRequest::addHeader(const std::string& headerKey,
                                              const std::string& headerValue) {
    Profile profile {"CurlRequest::addHeader"};
    if (_headerMap.find(headerKey) != _headerMap.end()) {
        _headerMap[headerKey] = headerValue;
        return fillAllHeaders();
    }

    _headerMap[headerKey] = headerValue;

    if (auto res = addToSlist(headerKey + ": " + headerValue); !res) {
        return res;
    }

    curl_easy_setopt(_handle, CURLOPT_HTTPHEADER, _headers);
    return {};
}
CurlClientResult<void> CurlRequest::setInstanceId(const std::string& instanceId) {
    Profile profile {"CurlRequest::setBearerToken"};

    if (_headerMap.find("Turing-Instance-Id") != _headerMap.end()) {
        _headerMap["Turing-Instance-Id"] = instanceId;
        return fillAllHeaders();
    }

    _headerMap["Turing-Instance-Id"] = instanceId;

    if (auto res = addToSlist("Turing-Instance-Id: " + _headerMap["Turing-Instance-Id"]); !res) {
        return res;
    }

    curl_easy_setopt(_handle, CURLOPT_HTTPHEADER, _headers);
    return {};
}

CurlClientResult<void> CurlRequest::setUrl(const std::string& url) {
    Profile profile {"CurlRequest::setUrl"};

    if (auto res = curl_easy_setopt(_handle, CURLOPT_URL, url.c_str()); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_URL, res);
    }
    return {};
}

CurlClientResult<void> CurlRequest::setPost(const std::string& postFields) {
    Profile profile {"CurlRequest::setPost"};

    if (auto res = curl_easy_setopt(_handle, CURLOPT_POST, 1L); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_POSTFIELDS, postFields.c_str()); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }

    if (auto res = curl_easy_setopt(_handle, CURLOPT_POSTFIELDSIZE, postFields.size()); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_POST_REQUEST, res);
    }

    return {};
}

CurlClientResult<void> CurlRequest::setWriteCallBack(WriteCallBackPointer func, void* userData) {
    Profile profile {"CurlRequest::setWriteCallBack"};

    if (auto res = curl_easy_setopt(_handle, CURLOPT_WRITEFUNCTION, func); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SET_WRITE_CALLBACK, res);
    }
    if (auto res = curl_easy_setopt(_handle, CURLOPT_WRITEDATA, userData); res != CURLE_OK) {
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
