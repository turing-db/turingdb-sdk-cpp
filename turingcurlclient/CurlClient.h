#pragma once

#include <vector>

#include "CurlRequest.h"
#include "RequestObject.h"

namespace turingClient {


class CurlClient {
public:
    CurlClient();
    void init();
    ~CurlClient();

    void sendRequest(RequestObject& req, WriteCallBack func);
    void sendRequest(CurlRequest&);
    CurlRequest& createRequest(RequestObject& req, WriteCallBack func);


private:
    std::vector<CurlRequest> _handles;
};


}
