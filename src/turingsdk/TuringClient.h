#pragma once

#include <vector>
#include <string>

#include "curl/CurlClient.h"
#include "TuringClientResult.h"

namespace turingsdk {

class TuringClientConfig;
class TypedColumn;

class TuringClient {
public:
    explicit TuringClient(const TuringClientConfig& config);

    ~TuringClient();

    TuringClientResult listAvailableGraphs(std::vector<std::string> &ret);
    TuringClientResult listLoadedGraphs(std::vector<std::string> &ret);

    TuringClientResult loadGraph(const std::string &graph);
    TuringClientResult history(std::vector<std::string> &ret,
                               const std::string &graph,
                               const std::string &commit = "",
                               const std::string &change = "");

    TuringClientResult query(const std::string &query, const std::string &graph,
                             std::vector<std::unique_ptr<TypedColumn>>& ret,
                             const std::string &commit = "",
                             const std::string &change = "");

private:
    const TuringClientConfig& _config;
    CurlClient _client;
};
}
