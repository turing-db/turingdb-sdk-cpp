#include "CurlClient.h"

#include <string.h>
#include <curl/curl.h>

#include "TuringException.h"

using namespace turingsdk;

namespace {

size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userdata) {
    CurlClient* client = static_cast<CurlClient*>(userdata);
    std::vector<char>& buffer = client->getBuffer();

    buffer.resize(size * nmemb);
    memcpy(buffer.data(), contents, size * nmemb);

    return size * nmemb;
}

}

CurlClient::CurlClient()
{
    _curl = curl_easy_init();
    if (!_curl) {
        throw TuringException("Failed to initialize CURL");
    }

    curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);
}

CurlClient::~CurlClient() {
    if (_curl) {
        curl_easy_cleanup(_curl);
    }
}

void CurlClient::setHeader(const std::string& headerKey, const std::string& headerValue) {
    _headers[headerKey] = headerValue;
}

void CurlClient::setUrl(const std::string& url) {
    curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
}

CurlClientResult CurlClient::send() {
    struct curl_slist* headers = nullptr;
    for (const auto& header : _headers) {
        headers = curl_slist_append(headers, (header.first + ": " + header.second).c_str());
    }

    curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);

    if (auto res = curl_easy_perform(_curl); res != CURLE_OK) {
        return CurlClientError::result(CurlClientErrorType::CANNOT_SEND_REQUEST, res);
    }

    if (headers) {
        curl_slist_free_all(headers);
    }

    return {};
}

void CurlClient::setPostData(const std::string& data) {
    curl_easy_setopt(_curl, CURLOPT_POST, 1L);
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, data.size());
}

void CurlClient::clearPostData() {
    curl_easy_setopt(_curl, CURLOPT_POST, 0L);
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, nullptr);
    curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, 0L);
}