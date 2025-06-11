#pragma once

#include "CurlClient.h"
#include "TuringRequestResult.h"
#include "CallBackSignatures.h"
#include "TypedColumn.h"
#include <memory>

namespace turingClient {

class TuringRequest {

public:
    explicit TuringRequest(std::string&);
    ~TuringRequest() = default;

    TuringRequestResult<void> listLoadedGraphs(std::vector<std::string>& result);
    TuringRequestResult<void> listAvailableGraphs(std::vector<std::string>& result);
    TuringRequestResult<void> loadGraph(std::string_view graph);
    TuringRequestResult<void> query(const std::string& query, const std::string& graph, std::vector<std::unique_ptr<TypedColumn>>& result);

private:
    CurlClient& _client;
    std::string& _url;
    std::vector<char> _buffer;
};
}
