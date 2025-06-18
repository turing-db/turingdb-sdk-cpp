#pragma once

#include "CurlClient.h"
#include "TuringClientResult.h"
#include "CallBackSignatures.h"
#include "TypedColumn.h"
#include <memory>

namespace turingClient {

class TuringClient {

    enum TuringClientOps : int8_t {
        LIST_AVAILBLE_GRAPHS,
        LIST_LOADED_GRAPHS,
        LOAD_GRAPH,
        QUERY,
        SIZE
    };

public:
    explicit TuringClient(std::string& url);
    ~TuringClient() = default;

    void init();

    const std::string& getUrl() { return _url; }
    void setUrl(std::string& url) { _url = std::move(url); }

    TuringClientError& getError() { return _result.error(); }

    bool listLoadedGraphs(std::vector<std::string>& result);
    bool listAvailableGraphs(std::vector<std::string>& result);
    bool loadGraph(const std::string& graph);
    bool query(const std::string& query,
               const std::string& graph,
               std::vector<std::unique_ptr<TypedColumn>>& result);

private:
    CurlClient& _client;
    CurlRequest& _handle;

    std::string _url;
    std::vector<char> _buffer;

    TuringClientResult<void> _result;
};
}
