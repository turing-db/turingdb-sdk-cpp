#include "TuringRequest.h"

#include "RequestObject.h"
#include "CurlClient.h"

#include <iostream>
#include <nlohmann/json.hpp>

using namespace turingClient;
using json = nlohmann::json;


TuringRequest::TuringRequest(std::string& url)
    :_url(url)
{
}

void TuringRequest::listAvailableGraphs(std::vector<std::string>& result) {
    RequestObject req = {HTTP_METHOD::POST, _url, "/list_avail_graphs"};
    auto func = [&result](char* ptr, size_t size, size_t nmemb, void* userdata) {
        json Doc {json::parse(ptr)};

        auto jsonVec = Doc[0]["data"].get<std::vector<std::string>>();
        result.insert(result.begin(), jsonVec.begin(), jsonVec.end());

        // Return the amount of data actually processed
        return size * nmemb;
    };

    _client.sendRequest(req, func);
}

void TuringRequest::listLoadedGraphs(std::vector<std::string>& result) {
    RequestObject req = {HTTP_METHOD::POST, _url, "/list_loaded_graphs"};
    auto func = [&result](char* ptr, size_t size, size_t nmemb, void* userdata) {
        json Doc {json::parse(ptr)};

        auto jsonVec = Doc[0]["data"][0][0].get<std::vector<std::string>>();
        result.insert(result.begin(), jsonVec.begin(), jsonVec.end());

        return size * nmemb;
    };

    _client.sendRequest(req, func);
}

void TuringRequest::loadGraph(std::string_view graph) {
    auto loadGraphURI = "/load_graph?graph=" + std::string(graph);
    RequestObject req = {HTTP_METHOD::POST, _url, loadGraphURI};
    auto func = [](char* ptr, size_t size, size_t nmemb, void* userdata) {
        return size * nmemb;
    };
    _client.sendRequest(req, func);
}

void parseJson(char* location, std::vector<std::unique_ptr<TypedColumn>>& result, size_t size) {
    json Doc {json::parse(location)};

    auto jsonData = Doc[0]["data"][0];
    auto colNames = Doc[0]["header"]["column_names"].get<std::vector<std::string>>();
    auto colTypes = Doc[0]["header"]["column_types"].get<std::vector<std::string>>();
    auto numCols = colNames.size();

    // TODO: check for errors here?
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
                // error unkown type
            }
        }
    }

    for (size_t i = 0; i < numCols; ++i) {
        switch (result[i].get()->column_type()) {
            case ColumnType::STRING: {
                auto* col = static_cast<Column<std::string>*>(result[i].get());
                for (const auto& val : jsonData[i]) {
                    if (!val.is_null()) {
                        col->push_back(val.get<std::string>());
                    } else {
                        col->push_back(std::nullopt);
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
                        col->push_back(std::nullopt);
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
                        col->push_back(std::nullopt);
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
                        col->push_back(std::nullopt);
                    }
                }
                break;
            }
            default:
                break;
        };
    }
}

void TuringRequest::query(std::string& query, std::string& graph, std::vector<std::unique_ptr<TypedColumn>>& result) {
    _buffer.clear();
    auto queryURI = "/query?graph=" + graph;
    RequestObject req = {HTTP_METHOD::POST, _url, queryURI, query};

    auto func = [this](char* ptr, size_t size, size_t nmemb, void* userdata) {
        size_t oldsize = _buffer.size();
        _buffer.resize(oldsize + size * nmemb);
        memcpy(_buffer.data() + oldsize, ptr, size * nmemb);

        return size * nmemb;
    };
    _client.sendRequest(req, func);
    _buffer.push_back('\0');
    parseJson(_buffer.data(), result, _buffer.size());
}
