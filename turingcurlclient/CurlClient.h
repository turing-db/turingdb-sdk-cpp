#pragma once

#include <vector>

#include "CurlRequest.h"

namespace turingClient {

class CurlClient {
public:
    CurlRequest& createHandle();

    std::vector<CurlRequest>& getHandles() { return _handles; }

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
