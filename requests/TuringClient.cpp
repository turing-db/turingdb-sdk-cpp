#include "TuringClient.h"

#include "Profiler.h"
#include "RequestObject.h"
#include "JsonUtils.h"
#include "CurlClient.h"

#include <nlohmann/json.hpp>

using namespace turingClient;
using json = nlohmann::json;

TuringClient::TuringClient(std::string& url)
    : _client(CurlClient::getCurlClient()),
    _url(std::move(url))
{
}

TuringClientResult<void> TuringClient::listAvailableGraphs(std::vector<std::string>& result) {
    RequestObject req = {HTTP_METHOD::POST, _url, "/list_avail_graphs"};

    TuringClientResult<void> ret;

    auto func = [&result, &ret](char* ptr, size_t size, size_t nmemb, void* userdata) {
        if (!ptr || !*ptr) {
            ret = TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
            return size * nmemb;
        }

        json res = json::parse(ptr, ptr + size * nmemb, nullptr, false);
        if (res.is_discarded()) {
            ret = TuringClientError::result(TuringClientErrorType::INVALID_JSON_FORMAT);
            return size * nmemb;
        }

        if (ret = checkJsonError(res, TuringClientErrorType::CANNOT_LIST_AVAILABLE_GRAPHS); !ret) {
            return size * nmemb;
        }

        if (!res.contains("data")) {
            ret = TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
            return size * nmemb;
        }

        auto jsonVec = res["data"].get<std::vector<std::string>>();
        result.insert(result.begin(), jsonVec.begin(), jsonVec.end());

        return size * nmemb;
    };

    if (auto res = _client.sendRequest(req, func); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_SEND_POST_REQUEST, res.error());
    }

    return ret;
}

TuringClientResult<void> TuringClient::listLoadedGraphs(std::vector<std::string>& result) {
    const RequestObject req = {HTTP_METHOD::POST, _url, "/list_loaded_graphs"};

    TuringClientResult<void> ret;

    auto func = [&result, &ret](char* ptr, size_t size, size_t nmemb, void* userdata) {
        if (!ptr || !*ptr) {
            ret = TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
            return size * nmemb;
        }

        json res = json::parse(ptr, ptr + size * nmemb, nullptr, false);
        if (res.is_discarded()) {
            ret = TuringClientError::result(TuringClientErrorType::INVALID_JSON_FORMAT);
            return size * nmemb;
        }

        if (ret = checkJsonError(res, TuringClientErrorType::CANNOT_LIST_LOADED_GRAPHS); !ret) {
            return size * nmemb;
        }

        if (!res.contains("data")) {
            ret = TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
            return size * nmemb;
        }
        auto jsonVec = res["data"][0][0].get<std::vector<std::string>>();
        result.insert(result.begin(), jsonVec.begin(), jsonVec.end());

        return size * nmemb;
    };

    if (auto res = _client.sendRequest(req, func); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_SEND_POST_REQUEST, res.error());
    }

    return ret;
}

TuringClientResult<void> TuringClient::loadGraph(const std::string& graph) {
    const auto loadGraphURI = "/load_graph?graph=" + graph;
    const RequestObject req = {HTTP_METHOD::POST, _url, loadGraphURI};

    TuringClientResult<void> ret;

    auto func = [&ret](char* ptr, size_t size, size_t nmemb, void* userdata) {
        if (!ptr || !*ptr) {
            ret = TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
            return size * nmemb;
        }

        json res = json::parse(ptr, ptr + size * nmemb, nullptr, false);
        if (res.is_discarded()) {
            ret = TuringClientError::result(TuringClientErrorType::INVALID_JSON_FORMAT);
            return size * nmemb;
        }

        ret = checkJsonError(res, TuringClientErrorType::CANNOT_LOAD_GRAPH);

        return size * nmemb;
    };

    if (auto res = _client.sendRequest(req, func); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_SEND_POST_REQUEST, res.error());
    }

    return ret;
}

TuringClientResult<void> TuringClient::query(const std::string& query,
                                             const std::string& graph,
                                             std::vector<std::unique_ptr<TypedColumn>>& result) {
    Profile profile {"TuringClient::query"};

    _buffer.clear();

    const auto queryURI = "/query?graph=" + graph;
    const RequestObject req = {HTTP_METHOD::POST, _url, queryURI, query};

    auto func = [this](char* ptr, size_t size, size_t nmemb, void* userdata) {
        size_t oldsize = _buffer.size();
        _buffer.resize(oldsize + size * nmemb);
        memcpy(_buffer.data() + oldsize, ptr, size * nmemb);

        return size * nmemb;
    };

    if (auto res = _client.sendRequest(req, func); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_SEND_POST_REQUEST, res.error());
    }

    _buffer.push_back('\0');
    return parseJson(_buffer.data(), result);

    return {};
}
