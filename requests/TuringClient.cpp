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
    _handle(_client.createHandle()),
    _url(std::move(url))
{
   init(); 
}

void TuringClient::init() {
}

TuringClientResult<void> TuringClient::listAvailableGraphs(std::vector<std::string>& result) {
    TuringClientResult<void> ret;

    WriteCallBack func = [&result, &ret](char* ptr, size_t size, size_t nmemb, void* userdata) {
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

    _handle.setUrl(_url + "/list_avail_graphs");
    if (auto res = _handle.setWriteCallBack(func); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_LIST_AVAILABLE_GRAPHS, res.error());
    }
    if (auto res = _handle.setPost(""); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_LIST_AVAILABLE_GRAPHS, res.error());
    }
    if (auto res = _handle.send(); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_LIST_AVAILABLE_GRAPHS, res.error());
    }


    return ret;
}

TuringClientResult<void> TuringClient::listLoadedGraphs(std::vector<std::string>& result) {
    TuringClientResult<void> ret;

    WriteCallBack func = [&result, &ret](char* ptr, size_t size, size_t nmemb, void* userdata) {
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

    _handle.setUrl(_url + "/list_loaded_graphs");
    if (auto res = _handle.setWriteCallBack(func); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_LIST_LOADED_GRAPHS, res.error());
    }
    if (auto res = _handle.setPost(""); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_LIST_LOADED_GRAPHS, res.error());
    }
    if (auto res = _handle.send(); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_LIST_LOADED_GRAPHS, res.error());
    }


    return ret;
}

TuringClientResult<void> TuringClient::loadGraph(const std::string& graph) {
    TuringClientResult<void> ret;

    WriteCallBack func = [&ret](char* ptr, size_t size, size_t nmemb, void* userdata) {
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

    if (auto res = _handle.setWriteCallBack(func); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_LOAD_GRAPH, res.error());
    }
    if (auto res = _handle.setUrl(_url + "/load_graph?graph=" + graph); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_LOAD_GRAPH, res.error());
    }
    if (auto res = _handle.setPost(""); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_LOAD_GRAPH, res.error());
    }
    if (auto res = _handle.send(); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_LOAD_GRAPH, res.error());
    }

    return ret;
}

TuringClientResult<void> TuringClient::query(const std::string& query,
                                             const std::string& graph,
                                             std::vector<std::unique_ptr<TypedColumn>>& result) {
    Profile profile {"TuringClient::query"};

    _buffer.clear();

    WriteCallBack func = [this](char* ptr, size_t size, size_t nmemb, void* userdata) {
        size_t oldsize = _buffer.size();
        _buffer.resize(oldsize + size * nmemb);
        memcpy(_buffer.data() + oldsize, ptr, size * nmemb);

        return size * nmemb;
    };

    if (auto res = _handle.setUrl(_url + "/query?graph=" + graph); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_QUERY_GRAPH, res.error());
    }
    if (auto res = _handle.setWriteCallBack(func); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_QUERY_GRAPH, res.error());
    }
    if (auto res = _handle.setPost(query); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_QUERY_GRAPH, res.error());
    }
    if (auto res = _handle.send(); !res) {
        return TuringClientError::result(TuringClientErrorType::CANNOT_QUERY_GRAPH, res.error());
    }

    _buffer.push_back('\0');
    return parseJson(_buffer.data(), result);

    return {};
}
