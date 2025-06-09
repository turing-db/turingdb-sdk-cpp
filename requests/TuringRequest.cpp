#include "TuringRequest.h"

#include "CurlClient.h"
#include "Profiler.h"
#include "RequestObject.h"

#include <nlohmann/json.hpp>

using namespace turingClient;
using json = nlohmann::json;


TuringRequest::TuringRequest(std::string& url)
    : _client(CurlClient::getCurlClient()),
    _url(url)
{
}

TuringRequestResult<void> checkJsonError(const json& jsonMsg, const TuringRequestErrorType retErrType) {
    if (auto queryErrorMsg = jsonMsg[0]["error"]; !queryErrorMsg) {
        return TuringRequestError::result(retErrType, queryErrorMsg.get<std::string>());
    }

    return {};
}

TuringRequestResult<void> TuringRequest::listAvailableGraphs(std::vector<std::string>& result) {
    RequestObject req = {HTTP_METHOD::POST, _url, "/list_avail_graphs"};

    TuringRequestResult<void> ret;

    auto func = [&result, &ret](char* ptr, size_t size, size_t nmemb, void* userdata) {
        json res {json::parse(ptr)};
        if (ret = checkJsonError(res, TuringRequestErrorType::CANNOT_LIST_AVAILABLE_GRAPHS); !ret) {
            return size * nmemb;
        }

        auto jsonVec = res[0]["data"].get<std::vector<std::string>>();
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
        json res {json::parse(ptr)};
        if (ret = checkJsonError(res, TuringRequestErrorType::CANNOT_LIST_LOADED_GRAPHS); !ret) {
            return size * nmemb;
        }

        auto jsonVec = res[0]["data"][0][0].get<std::vector<std::string>>();
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
        const json res {json::parse(ptr)};
        ret = checkJsonError(res, TuringRequestErrorType::CANNOT_LOAD_GRAPH);

        return size * nmemb;
    };

    if (auto res = _client.sendRequest(req, func); !res) {
        return TuringRequestError::result(TuringRequestErrorType::CANNOT_SEND_POST_REQUEST, res.error());
    }

    return ret;
}

TuringRequestResult<void> parseJson(char* location, std::vector<std::unique_ptr<TypedColumn>>& result, size_t size) {
    Profile profile {"TuringRequest::parseJson"};

    Profile* jsonParseProfile = new Profile("json Parsing");
    const json res {json::parse(location)};
    delete jsonParseProfile;

    if (auto ret = checkJsonError(res, turingClient::TuringRequestErrorType::TURING_QUERY_FAILED); !ret) {
        return ret;
    }

    const auto jsonHeader = res[0]["header"];
    const auto colNames = res[0]["header"]["column_names"].get<std::vector<std::string>>();
    const auto colTypes = res[0]["header"]["column_types"].get<std::vector<std::string>>();
    const auto numCols = colNames.size();

    if (result.empty()) {
        for (size_t i = 0; i < numCols; ++i) {
            if (colTypes[i] == "String") {
                result.emplace_back(std::make_unique<Column<std::string>>(colNames[i]));
            } else if (colTypes[i] == "Bool") {
                result.emplace_back(std::make_unique<Column<CustomBool>>(colNames[i]));
            } else if (colTypes[i] == "UInt64") {
                result.emplace_back(std::make_unique<Column<uint64_t>>(colNames[i]));
            } else if (colTypes[i] == "Int64") {
                result.emplace_back(std::make_unique<Column<int64_t>>(colNames[i]));
            } else {
                return TuringRequestError::result(TuringRequestErrorType::UNKOWN_COLUMN_TYPE);
            }
        }
    }

    for (const auto& jsonData : res[0]["data"]) {
        for (size_t i = 0; i < numCols; ++i) {
            switch (result[i].get()->columnType()) {
                case ColumnType::STRING: {
                    auto* col = static_cast<Column<std::string>*>(result[i].get());
                    for (const auto& val : jsonData[i]) {
                        if (!val.is_null()) {
                            col->push_back(val.get<std::string>());
                        } else {
                            col->pushNull();
                        }
                    }
                    break;
                }
                case ColumnType::BOOL: {
                    auto* col = static_cast<Column<CustomBool>*>(result[i].get());
                    for (const auto& val : jsonData[i]) {
                        if (!val.is_null()) {
                            col->push_back(val.get<bool>());
                        } else {
                            col->pushNull();
                        }
                    }
                    break;
                }
                case ColumnType::UINT: {
                    auto* col = static_cast<Column<uint64_t>*>(result[i].get());
                    for (const auto& val : jsonData[i]) {
                        if (!val.is_null()) {
                            col->push_back(val.get<uint64_t>());
                        } else {
                            col->pushNull();
                        }
                    }
                    break;
                }
                case ColumnType::INT: {
                    auto* col = static_cast<Column<int64_t>*>(result[i].get());
                    for (const auto& val : jsonData[i]) {
                        if (!val.is_null()) {
                            col->push_back(val.get<int64_t>());
                        } else {
                            col->pushNull();
                        }
                    }
                    break;
                }
                default:
                    break;
            };
        }
    }

    return {};
}

TuringRequestResult<void> TuringRequest::query(const std::string& query, const std::string& graph,
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
    return parseJson(_buffer.data(), result, _buffer.size());

    return {};
}
