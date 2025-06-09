#pragma once

#include <vector>

#include "CurlRequest.h"
#include "CurlClientResult.h"
#include "RequestObject.h"

namespace turingClient {

class CurlClient {
public:
    CurlClientResult<void> sendRequest(const RequestObject& req, WriteCallBack func);

    static CurlClient& getCurlClient() {
        static CurlClient client;
        return client;
    }

private:
    CurlClient();
    CurlClient(CurlClient&) = delete;
    CurlClient(CurlClient&&) = delete;
    ~CurlClient();

    void init();

    std::vector<CurlRequest> _handles;
};


}
