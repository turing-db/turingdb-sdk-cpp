#include "TuringRequest.h"

#include "Profiler.h"
#include "RequestObject.h"
#include "JsonUtils.h"
#include "CurlClient.h"

#include <nlohmann/json.hpp>

using namespace turingClient;
using json = nlohmann::json;

TuringRequest::TuringRequest(const std::string& url)
    : _client(CurlClient::getCurlClient()),
    _url(url)
{
}

TuringRequestResult<void> TuringRequest::listAvailableGraphs(std::vector<std::string>& result) {
    RequestObject req = {HTTP_METHOD::POST, _url, "/list_avail_graphs"};

    TuringRequestResult<void> ret;

    auto func = [&result, &ret](char* ptr, size_t size, size_t nmemb, void* userdata) {
        if (!ptr || !*ptr) {
            ret = TuringRequestError::result(TuringRequestErrorType::UNKNOWN_JSON_FORMAT);
            return size * nmemb;
        }

        json res = json::parse(ptr, ptr + size * nmemb, nullptr, false);
        if (res.is_discarded()) {
            ret = TuringRequestError::result(TuringRequestErrorType::INVALID_JSON_FORMAT);
            return size * nmemb;
        }

        if (ret = checkJsonError(res, TuringRequestErrorType::CANNOT_LIST_AVAILABLE_GRAPHS); !ret) {
            return size * nmemb;
        }

        if (!res.contains("data")) {
            ret = TuringRequestError::result(TuringRequestErrorType::UNKNOWN_JSON_FORMAT);
            return size * nmemb;
        }

        auto jsonVec = res["data"].get<std::vector<std::string>>();
        result.insert(result.begin(), jsonVec.begin(), jsonVec.end());

        return size * nmemb;
    };

    if (auto res = _client.sendRequest(req, func); !res) {
        return TuringRequestError::result(TuringRequestErrorType::CANNOT_SEND_POST_REQUEST, res.error());
    }

    return ret;
}

TuringRequestResult<void> TuringRequest::listLoadedGraphs(std::vector<std::string>& result) {
    const RequestObject req = {HTTP_METHOD::POST, _url, "/list_loaded_graphs"};

    TuringRequestResult<void> ret;

    auto func = [&result, &ret](char* ptr, size_t size, size_t nmemb, void* userdata) {
        if (!ptr || !*ptr) {
            ret = TuringRequestError::result(TuringRequestErrorType::UNKNOWN_JSON_FORMAT);
            return size * nmemb;
        }

        json res = json::parse(ptr, ptr + size * nmemb, nullptr, false);
        if (res.is_discarded()) {
            ret = TuringRequestError::result(TuringRequestErrorType::INVALID_JSON_FORMAT);
            return size * nmemb;
        }

        if (ret = checkJsonError(res, TuringRequestErrorType::CANNOT_LIST_LOADED_GRAPHS); !ret) {
            return size * nmemb;
        }

        if (!res.contains("data")) {
            ret = TuringRequestError::result(TuringRequestErrorType::UNKNOWN_JSON_FORMAT);
            return size * nmemb;
        }
        auto jsonVec = res["data"][0][0].get<std::vector<std::string>>();
        result.insert(result.begin(), jsonVec.begin(), jsonVec.end());

        return size * nmemb;
    };

    if (auto res = _client.sendRequest(req, func); !res) {
        return TuringRequestError::result(TuringRequestErrorType::CANNOT_SEND_POST_REQUEST, res.error());
    }

    return ret;
}

TuringRequestResult<void> TuringRequest::loadGraph(std::string_view graph) {
    const auto loadGraphURI = "/load_graph?graph=" + std::string(graph);
    const RequestObject req = {HTTP_METHOD::POST, _url, loadGraphURI};

    TuringRequestResult<void> ret;

    auto func = [&ret](char* ptr, size_t size, size_t nmemb, void* userdata) {
        if (!ptr || !*ptr) {
            ret = TuringRequestError::result(TuringRequestErrorType::UNKNOWN_JSON_FORMAT);
            return size * nmemb;
        }

        json res = json::parse(ptr, ptr + size * nmemb, nullptr, false);
        if (res.is_discarded()) {
            ret = TuringRequestError::result(TuringRequestErrorType::INVALID_JSON_FORMAT);
            return size * nmemb;
        }

        ret = checkJsonError(res, TuringRequestErrorType::CANNOT_LOAD_GRAPH);

        return size * nmemb;
    };

    if (auto res = _client.sendRequest(req, func); !res) {
        return TuringRequestError::result(TuringRequestErrorType::CANNOT_SEND_POST_REQUEST, res.error());
    }

    return ret;
}

TuringRequestResult<void> TuringRequest::query(const std::string& query,
                                               const std::string& graph,
                                               std::vector<std::unique_ptr<TypedColumn>>& result) {
    Profile profile {"TuringRequest::query"};

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
        return TuringRequestError::result(TuringRequestErrorType::CANNOT_SEND_POST_REQUEST, res.error());
    }

    _buffer.push_back('\0');
    return parseJson(_buffer.data(), result);

    return {};
}
