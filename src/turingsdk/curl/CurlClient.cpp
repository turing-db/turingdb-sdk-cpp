#include "CurlClient.h"

#include <curl/curl.h>

#include <string>

using namespace turingsdk;

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

CurlRequest* CurlClient::createHandle() {
    return _handles.emplace_back(std::make_unique<CurlRequest>()).get();
}
