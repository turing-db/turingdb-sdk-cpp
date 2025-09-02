#include "TuringClient.h"

#include "Profiler.h"
#include "JsonUtils.h"
#include "curl/CurlClient.h"

#include <nlohmann/json.hpp>

using namespace turingClient;
using json = nlohmann::json;

TuringClient::TuringClient(const std::string& url)
    : _client(CurlClient::getCurlClient()),
    _handle(_client.createHandle()),
    _url(url)
{
}

TuringClient::TuringClient(std::string&& url)
    : _client(CurlClient::getCurlClient()),
    _handle(_client.createHandle()),
    _url(std::move(url))
{
}

bool TuringClient::setBearerToken(const std::string& token) {
    Profile profile {"TuringClient::setBearerToken"};
    if (auto res = _handle->addHeader("Authorization", "Bearer " + token); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_SET_BEARER_TOKEN, res.error());
        return false;
    }

    return true;
}

void TuringClient::removeBearerToken() {
    Profile profile {"TuringClient::removeBearerToken"};
    _handle->clearHeader("Authorization");
}

bool TuringClient::setInstanceId(const std::string& instanceId) {
    Profile profile {"TuringClient::setInstanceId"};
    if (auto res = _handle->addHeader("Turing-Instance-Id", instanceId); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_SET_BEARER_TOKEN, res.error());
        return false;
    }

    return true;
}

void TuringClient::removeInstanceId() {
    Profile profile {"TuringClient::removeInstanceId"};
    _handle->clearHeader("Turing-Instance-Id");
}

bool TuringClient::listAvailableGraphs(std::vector<std::string>& ret) {
    Profile profile {"TuringClient::listLoadedGraphs"};

    auto vals = std::make_unique<callbackReturnValues>(_result, ret);
    if (auto res = _handle->setUrl(_url + "/list_avail_graphs"); !res) {
        _result = TuringClientError::result(TuringClientErrorType::CANNOT_LIST_AVAILABLE_GRAPHS, res.error());
        return false;
    }
    if (auto res = _handle->setWriteCallBack(listAvailableGraphsCallBack,
                                             static_cast<void*>(vals.get())); !res) {
        this->_result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LIST_AVAILABLE_GRAPHS, res.error());
        return false;
    }
    if (auto res = _handle->setPost(""); !res) {
        this->_result = TuringClientError::result(TuringClientErrorType::CANNOT_LIST_AVAILABLE_GRAPHS, res.error());
        return false;
    }
    if (auto res = _handle->send(); !res) {
        this->_result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LIST_AVAILABLE_GRAPHS, res.error());
        return false;
    }

    return _result.has_value();
}

bool TuringClient::listLoadedGraphs(std::vector<std::string>& ret) {
    Profile profile {"TuringClient::listLoadedGraphs"};

    auto vals = std::make_unique<callbackReturnValues>(_result, ret);
    if (auto res = _handle->setUrl(_url + "/list_loaded_graphs"); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LIST_LOADED_GRAPHS, res.error());
        return false;
    }
    if (auto res = _handle->setWriteCallBack(listLoadedGraphsCallBack,
                                             static_cast<void*>(vals.get())); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LIST_LOADED_GRAPHS, res.error());
        return false;
    }
    if (auto res = _handle->setPost(""); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LIST_LOADED_GRAPHS, res.error());
        return false;
    }
    if (auto res = _handle->send(); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LIST_LOADED_GRAPHS, res.error());
        return false;
    }

    return _result.has_value();
}

bool TuringClient::loadGraph(const std::string& graph) {
    Profile profile {"TuringClient::loadGraph"};

    if (auto res = _handle->setWriteCallBack(loadGraphCallBack,
                                             static_cast<void*>(&_result)); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LOAD_GRAPH, res.error());
        return false;
    }
    if (auto res = _handle->setUrl(_url + "/load_graph?graph=" + graph); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LOAD_GRAPH, res.error());
        return false;
    }
    if (auto res = _handle->setPost(""); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LOAD_GRAPH, res.error());
        return false;
    }
    if (auto res = _handle->send(); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LOAD_GRAPH, res.error());
        return false;
    }

    return _result.has_value();
}

void parameterBuilder(std::string& queryString,
                      const std::string& graph,
                      const std::string& commit,
                      const std::string& change) {
    queryString += "graph=" + graph;
    if (!commit.empty()) {
        queryString += "&commit=" + commit;
    }
    if (!change.empty()) {
        queryString += "&change=" + change;
    }
}
bool TuringClient::query(const std::string& query,
                         const std::string& graph,
                         std::vector<std::unique_ptr<TypedColumn>>& ret,
                         const std::string& commit,
                         const std::string& change) {
    Profile profile {"TuringClient::query"};

    _buffer.clear();

    WriteCallBack func = [this](char* ptr, size_t size, size_t nmemb,
                                void* userdata) {
        size_t oldsize = _buffer.size();
        _buffer.resize(oldsize + size * nmemb);
        memcpy(_buffer.data() + oldsize, ptr, size * nmemb);

        return size * nmemb;
    };

    std::string urlParameters = "/query?";
    parameterBuilder(urlParameters, graph, commit, change);

    if (auto res = _handle->setUrl(_url + urlParameters); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_QUERY_GRAPH, res.error());
        return false;
    }

    if (auto res = _handle->setWriteCallBack(queryCallBack, static_cast<void*>(&_buffer)); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_QUERY_GRAPH, res.error());
        return false;
    }

    if (auto res = _handle->setPost(query); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_QUERY_GRAPH, res.error());
        return false;
    }

    if (auto res = _handle->send(); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_QUERY_GRAPH, res.error());
        return false;
    }

    _buffer.push_back('\0');
    _result = parseJson(_buffer.data(), ret);

    return _result.has_value();
}

bool TuringClient::history(std::vector<std::string>& ret,
                           const std::string& graph,
                           const std::string& commit,
                           const std::string& change) {
    Profile profile {"TuringClient::history"};

    auto vals = std::make_unique<callbackReturnValues>(_result, ret);

    std::string urlParameters = "/history?";
    parameterBuilder(urlParameters, graph, commit, change);

    if (auto res = _handle->setUrl(_url + urlParameters); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LOAD_HISTORY, res.error());
        return false;
    }

    if (auto res = _handle->setWriteCallBack(historyCallBack,
                                             static_cast<void*>(vals.get()));
        !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LOAD_HISTORY, res.error());
        return false;
    }

    if (auto res = _handle->setPost(""); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LOAD_HISTORY, res.error());
        return false;
    }

    if (auto res = _handle->send(); !res) {
        _result = TuringClientError::result(
            TuringClientErrorType::CANNOT_LOAD_HISTORY, res.error());
        return false;
    }

    return _result.has_value();
}

size_t TuringClient::listAvailableGraphsCallBack(char* ptr, size_t size,
                                                 size_t nmemb, void* userdata) {
    auto* resultVals = static_cast<callbackReturnValues*>(userdata);
    if (!ptr || !*ptr) {
        resultVals->errorResult =
            TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
        return size * nmemb;
    }

    json res = json::parse(ptr, ptr + size * nmemb, nullptr, false);
    if (res.is_discarded()) {
        resultVals->errorResult =
            TuringClientError::result(TuringClientErrorType::INVALID_JSON_FORMAT);
        return size * nmemb;
    }

    if (resultVals->errorResult = checkJsonError(
            res, TuringClientErrorType::CANNOT_LIST_AVAILABLE_GRAPHS);
        !resultVals->errorResult) {
        return size * nmemb;
    }

    if (!res.contains("data")) {
        resultVals->errorResult =
            TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
        return size * nmemb;
    }

    auto jsonVec = res["data"].get<std::vector<std::string>>();
    resultVals->stringResult.insert(resultVals->stringResult.begin(),
                                    jsonVec.begin(), jsonVec.end());

    return size * nmemb;
}

size_t TuringClient::historyCallBack(char* ptr, size_t size, size_t nmemb,
                                     void* userdata) {
    auto* const resultVals = static_cast<callbackReturnValues*>(userdata);
    if (!ptr || !*ptr) {
        resultVals->errorResult =
            TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
        return size * nmemb;
    }

    const json res = json::parse(ptr, ptr + size * nmemb, nullptr, false);
    if (res.is_discarded()) {
        resultVals->errorResult =
            TuringClientError::result(TuringClientErrorType::INVALID_JSON_FORMAT);
        return size * nmemb;
    }

    if (resultVals->errorResult =
            checkJsonError(res, TuringClientErrorType::CANNOT_LOAD_HISTORY);
        !resultVals->errorResult) {
        return size * nmemb;
    }

    const auto dataIt = res.find("data");
    if (dataIt == res.end()) {
        resultVals->errorResult =
            TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
        return size * nmemb;
    }

    const auto commitsIt = dataIt->find("commits");
    if (commitsIt == dataIt->end() || !(*commitsIt).is_array()) {
        resultVals->errorResult =
            TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
        return size * nmemb;
    }

    const auto& jsonVec = (*commitsIt).get<std::vector<std::string>>();
    resultVals->stringResult.insert(resultVals->stringResult.begin(),
                                    jsonVec.begin(), jsonVec.end());

    return size * nmemb;
}

size_t TuringClient::listLoadedGraphsCallBack(char* ptr, size_t size,
                                              size_t nmemb, void* userdata) {
    auto* const resultVals = static_cast<callbackReturnValues*>(userdata);
    if (!ptr || !*ptr) {
        resultVals->errorResult =
            TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
        return size * nmemb;
    }

    const json res = json::parse(ptr, ptr + size * nmemb, nullptr, false);
    if (res.is_discarded()) {
        resultVals->errorResult =
            TuringClientError::result(TuringClientErrorType::INVALID_JSON_FORMAT);
        return size * nmemb;
    }

    if (resultVals->errorResult =
            checkJsonError(res, TuringClientErrorType::CANNOT_LIST_LOADED_GRAPHS);
        !resultVals->errorResult) {
        return size * nmemb;
    }

    if (!res.contains("data")) {
        resultVals->errorResult =
            TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
        return size * nmemb;
    }
    const auto& jsonVec = res["data"][0][0].get<std::vector<std::string>>();
    resultVals->stringResult.insert(resultVals->stringResult.begin(),
                                    jsonVec.begin(), jsonVec.end());

    return size * nmemb;
}

size_t TuringClient::loadGraphCallBack(char* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* const errorResult = static_cast<TuringClientResult<void>*>(userdata);
    if (!ptr || !*ptr) {
        *errorResult = TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
        return size * nmemb;
    }

    json res = json::parse(ptr, ptr + size * nmemb, nullptr, false);
    if (res.is_discarded()) {
        *errorResult = TuringClientError::result(TuringClientErrorType::INVALID_JSON_FORMAT);
        return size * nmemb;
    }

    *errorResult = checkJsonError(res, TuringClientErrorType::CANNOT_LOAD_GRAPH);

    return size * nmemb;
}

size_t TuringClient::queryCallBack(char* ptr, size_t size, size_t nmemb,
                                   void* userdata) {
    auto* const buffer = static_cast<std::vector<char>*>(userdata);
    size_t oldsize = buffer->size();
    buffer->resize(oldsize + size * nmemb);
    memcpy(buffer->data() + oldsize, ptr, size * nmemb);

    return size * nmemb;
}
