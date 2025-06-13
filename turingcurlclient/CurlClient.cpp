#include "CurlClient.h"
#include <curl/curl.h>
#include <iostream>
#include <string>

#include "Profiler.h"

using namespace turingClient;

CurlClient::CurlClient()
{
    init();
}

void CurlClient::init() {
    Profile profile {"CurlClient::init"};
    curl_global_init(CURL_GLOBAL_ALL);
}

CurlClient::~CurlClient() {
    _handles.clear();
    curl_global_cleanup();
}

CurlClientResult<void> CurlClient::sendRequest(const RequestObject& req,
                                               WriteCallBack func) {
    Profile profile {"CurlClient::sendRequest"};
    CurlRequest& curlReq = _handles.emplace_back();

    switch (req.method) {
        case HTTP_METHOD::POST:
            if (auto res = curlReq.setPost(req.body); !res) {
                return res;
            }
            break;
        default:
            return CurlClientError::result(CurlClientErrorType::UNKNOWN_HTTP_METHOD);
    }

    auto fullUrl = std::string(req.url) + std::string(req.endpoint);
    if (auto res = curlReq.setUrl(fullUrl); !res) {
        return res;
    }

    if (auto res = curlReq.setWriteCallBack(func); !res) {
        return res;
    }

    if (auto res = curlReq.send(); !res) {
        return res;
    }

    return {};
}
