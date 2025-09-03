#pragma once

#include <vector>

#include "CurlRequest.h"

namespace turingsdk {

class CurlClient {
public:
    CurlRequest* createHandle();

    std::vector<std::unique_ptr<CurlRequest>>& getHandles() { return _handles; }

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

    std::vector<std::unique_ptr<CurlRequest>> _handles;
};
}
