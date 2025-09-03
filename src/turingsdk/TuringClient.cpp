#include "TuringClient.h"

#include <nlohmann/json.hpp>

#include "TuringClientConfig.h"
#include "JsonUtils.h"

using namespace turingsdk;

TuringClient::TuringClient(const TuringClientConfig& config)
    : _config(config)
{
    _client.setHeader("Authorization", "Bearer " + _config.getBearerToken());
    _client.setHeader("Turing-Instance-Id", _config.getInstanceID());
}

TuringClient::~TuringClient() {
}

TuringClientResult TuringClient::listAvailableGraphs(std::vector<std::string>& ret) {
    _client.setUrl(_config.getHostURL() + "/list_avail_graphs");
    
    if (auto res = _client.send(); !res) {
        return TuringClientError::result(
            TuringClientErrorType::CANNOT_LIST_AVAILABLE_GRAPHS, res.error());
    }

    const auto jsonRes = nlohmann::json::parse(_client.getBuffer().data(), nullptr, false);
    if (jsonRes.is_discarded()) {
        return TuringClientError::result(TuringClientErrorType::INVALID_JSON_FORMAT);
    }

    if (auto res = JsonUtils::checkJsonError(jsonRes,TuringClientErrorType::CANNOT_LIST_AVAILABLE_GRAPHS); !res) {
        return res;
    }

    if (!jsonRes.contains("data")) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    const std::vector<std::string>& jsonVec = jsonRes["data"].get<std::vector<std::string>>();
    ret.insert(ret.begin(),
        jsonVec.begin(), jsonVec.end());

    return {};
}

TuringClientResult TuringClient::listLoadedGraphs(std::vector<std::string>& ret) {
    _client.setUrl(_config.getHostURL() + "/list_loaded_graphs");
    
    if (auto res = _client.send(); !res) {
        return TuringClientError::result(
            TuringClientErrorType::CANNOT_LIST_LOADED_GRAPHS, res.error());
    }

    const auto jsonRes = nlohmann::json::parse(_client.getBuffer().data(), nullptr, false);
    if (jsonRes.is_discarded()) {
        return TuringClientError::result(TuringClientErrorType::INVALID_JSON_FORMAT);
    }

    if (auto res = JsonUtils::checkJsonError(jsonRes, TuringClientErrorType::CANNOT_LIST_LOADED_GRAPHS); !res) {
        return res;
    }

    if (!jsonRes.contains("data")) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    const std::vector<std::string>& jsonVec = jsonRes["data"].get<std::vector<std::string>>();
    ret.insert(ret.begin(),
        jsonVec.begin(), jsonVec.end());

    return {};
}

TuringClientResult TuringClient::loadGraph(const std::string& graph) {
    _client.setUrl(_config.getHostURL() + "/load_graph?graph=" + graph);
    
    if (auto res = _client.send(); !res) {
        return TuringClientError::result(
            TuringClientErrorType::CANNOT_LOAD_GRAPH, res.error());
    }

    return {};
}

TuringClientResult TuringClient::query(const std::string& query,
                                       const std::string& graph,
                                       std::vector<std::unique_ptr<TypedColumn>>& ret,
                                       const std::string& commit,
                                       const std::string& change) {
    // Build URL
    std::string url = _config.getHostURL() + "/query?";
    if (!commit.empty()) {
        url += "&commit=" + commit;
    }
    if (!change.empty()) {
        url += "&change=" + change;
    }
    url += "&graph=" + graph;

    _client.setUrl(url);

    // Set post data
    _client.setPostData(query);

    // Send request
    if (auto res = _client.send(); !res) {
        return TuringClientError::result(
            TuringClientErrorType::CANNOT_QUERY_GRAPH, res.error());
    }

    auto& buffer = _client.getBuffer();
    buffer.push_back('\0');

    // Parse JSON
    if (auto res = JsonUtils::parseJson(buffer.data(), ret); !res) {
        return res;
    }

    _client.clearPostData();

    return {};
}

TuringClientResult TuringClient::history(std::vector<std::string>& ret,
                                         const std::string& graph,
                                         const std::string& commit,
                                         const std::string& change) {
    std::string url = _config.getHostURL() + "/history?";
    if (!commit.empty()) {
        url += "&commit=" + commit;
    }
    if (!change.empty()) {
        url += "&change=" + change;
    }
    url += "&graph=" + graph;

    _client.setUrl(url);

    if (auto res = _client.send(); !res) {
        return TuringClientError::result(
            TuringClientErrorType::CANNOT_LOAD_HISTORY, res.error());
    }

    const auto jsonRes = nlohmann::json::parse(_client.getBuffer().data(), nullptr, false);
    if (jsonRes.is_discarded()) {
        return TuringClientError::result(TuringClientErrorType::INVALID_JSON_FORMAT);
    }

    if (auto res = JsonUtils::checkJsonError(jsonRes, TuringClientErrorType::CANNOT_LOAD_HISTORY); !res) {
        return res;
    }

    const auto dataIt = jsonRes.find("data");
    if (dataIt == jsonRes.end()) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    const auto commitsIt = dataIt->find("commits");
    if (commitsIt == dataIt->end() || !(*commitsIt).is_array()) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    const std::vector<std::string>& jsonVec = (*commitsIt).get<std::vector<std::string>>();
    ret.insert(ret.begin(),
        jsonVec.begin(), jsonVec.end());

    return {};
}
