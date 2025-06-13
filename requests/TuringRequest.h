#pragma once

#include "CurlClient.h"
#include "TuringRequestResult.h"
#include "CallBackSignatures.h"
#include "TypedColumn.h"
#include <memory>

namespace turingClient {

class TuringRequest {

public:
    explicit TuringRequest(std::string& url);
    ~TuringRequest() = default;

    void setUrl(std::string& url) { _url = std::move(url); }

    TuringRequestResult<void> listLoadedGraphs(std::vector<std::string>& result);
    TuringRequestResult<void> listAvailableGraphs(std::vector<std::string>& result);
    TuringRequestResult<void> loadGraph(const std::string& graph);
    TuringRequestResult<void> query(const std::string& query,
                                    const std::string& graph,
                                    std::vector<std::unique_ptr<TypedColumn>>& result);

private:
    CurlClient& _client;
    std::string _url;
    std::vector<char> _buffer;
};
}
