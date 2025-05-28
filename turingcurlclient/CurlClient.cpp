#include "CurlClient.h"
#include <curl/curl.h>
#include <iostream>
#include <string>

using namespace turingClient;

CurlClient::CurlClient()
{
    init();
}

void CurlClient::init() {
    curl_global_init(CURL_GLOBAL_ALL);
}

CurlClient::~CurlClient() {
    _handles.clear();
    curl_global_cleanup();
}

void CurlClient::sendRequest(RequestObject& req, WriteCallBack func) {
    CurlRequest& curlReq = _handles.emplace_back();

    switch (req.method) {
        case HTTP_METHOD::POST:
            if (auto res = curlReq.setPost(req.body); res) {
                std::cout << "Couldn't set post" << std::endl;
                return;
            }
            break;
        default:
            std::cout << "Couldn't recognise anything" << std::endl;
            return;
    }

    auto fullUrl = std::string(req.url) + std::string(req.endpoint);
    if (auto res = curlReq.setUrl(fullUrl); res) {
        std::cout << "couldn't set url" << std::endl;
        return;
    }

    if (auto res = curlReq.setWriteCallBack(func); res) {
        std::cout << "couldn't set callback" << std::endl;
        return;
    }

    if (auto res = curlReq.send(); res) {
        return;
    }
}
