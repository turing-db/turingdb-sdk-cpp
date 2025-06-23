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

CurlRequest* CurlClient::createHandle() {
    return _handles.emplace_back().get();
}
