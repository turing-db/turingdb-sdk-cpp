#pragma once

#include "CurlClient.h"
#include "CallBackSignatures.h"

namespace turingClient {

class TuringRequest {
public:
    TuringRequest() = default;
    ~TuringRequest() = default;

    void listLoadedGraphs(WriteCallBack func);
    void listAvailableGraphs(WriteCallBack func);

private:
    CurlClient _client;
};

}
