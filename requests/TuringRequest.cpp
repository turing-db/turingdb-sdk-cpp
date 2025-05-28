#include "TuringRequest.h"

#include "RequestObject.h"
#include "CurlClient.h"

#include <iostream>
#include <nlohmann/json.hpp>

using namespace turingClient;
using json = nlohmann::json;



void TuringRequest::listAvailableGraphs(std::vector<std::string>& result) {
    RequestObject req = {HTTP_METHOD::POST, "http://127.0.0.1:6666", "/list_avail_graphs"};
    auto func = [&result](char* ptr, size_t size, size_t nmemb, void* userdata) {
        // std::cout.write(ptr, size * nmemb);
        // std::cout << std::endl;

        json Doc {json::parse(ptr)};

        std::cout << Doc.dump() << std::endl;
        auto jsonVec = Doc[0]["data"].get<std::vector<std::string>>();
        result.insert(result.begin(), jsonVec.begin(), jsonVec.end());

        // Return the amount of data actually processed
        return size * nmemb;
    };

    _client.sendRequest(req, func);
}

void TuringRequest::listLoadedGraphs(std::vector<std::string>& result) {
    RequestObject req = {HTTP_METHOD::POST, "127.0.0.1:6666", "/list_loaded_graphs"};
    auto func = [&result](char* ptr, size_t size, size_t nmemb, void* userdata) {
        json Doc {json::parse(ptr)};

        std::cout << Doc.dump() << std::endl;
        auto jsonVec = Doc[0]["data"].get<std::vector<std::string>>();
        result.insert(result.begin(), jsonVec.begin(), jsonVec.end());

        return size * nmemb;
    };

    _client.sendRequest(req, func);
}

void TuringRequest::loadGraph(std::string_view graph) {
    auto loadGraphURI = "/load_graph?graph=" + std::string(graph);
    RequestObject req = {HTTP_METHOD::POST, "127.0.0.1:6666", loadGraphURI};
    auto func = [](char* ptr, size_t size, size_t nmemb, void* userdata) {
        return size * nmemb;
    };
    _client.sendRequest(req, func);
}

void TuringRequest::query(std::string& query, std::string& graph, std::vector<std::unique_ptr<TypedColumn>>& result) {
    auto queryURI = "/query?graph=" + graph;
    RequestObject req = {HTTP_METHOD::POST, "127.0.0.1:6666", queryURI, query};

    auto func = [&result](char* ptr, size_t size, size_t nmemb, void* userdata) {
        json Doc {json::parse(ptr)};

        std::cout << Doc.dump() << std::endl;
        auto jsonData = Doc[0]["data"][0];
        auto numCols = jsonData.size();
        std::cout << numCols << std::endl;
        // check for errors here?
        if (result.empty()) {
            for (size_t i = 0; i < numCols; ++i) {
                std::cout << jsonData[i].dump() << std::endl;
                std::cout << jsonData[i][0].type_name() << std::endl;
                switch (jsonData[i][0].type()) {
                    case nlohmann::detail::value_t::string:
                        std::cout << "string" << std::endl;
                        result.push_back(std::make_unique<Column<std::string>>(jsonData[i].get<std::vector<std::string>>()));
                        break;
                    case nlohmann::detail::value_t::boolean:
                        std::cout << "bool" << std::endl;
                        result.push_back(std::make_unique<Column<CustomBool>>(jsonData[i].get<std::vector<bool>>()));
                        break;
                    case nlohmann::detail::value_t::number_unsigned:
                        std::cout << "int" << std::endl;
                        result.push_back(std::make_unique<Column<uint64_t>>(jsonData[i].get<std::vector<uint64_t>>()));
                        break;
                    case nlohmann::detail::value_t::number_integer:
                        std::cout << "int" << std::endl;
                        result.push_back(std::make_unique<Column<int64_t>>(jsonData[i].get<std::vector<int64_t>>()));
                        break;
                    default:
                        break;
                };
            }
            return size * nmemb;
        }

        for (size_t i = 0; i < numCols; ++i) {
            switch (jsonData[i][0].type()) {
                case nlohmann::detail::value_t::string: {
                    auto* col = static_cast<Column<std::string>*>(result[i].get());
                    auto jsonCol = jsonData[i].get<std::vector<std::string>>();
                    col->insert(col->end(), jsonCol.begin(), jsonCol.end());
                    break;
                }
                case nlohmann::detail::value_t::boolean: {
                    auto* col = static_cast<Column<CustomBool>*>(result[i].get());
                    auto jsonCol = jsonData[i].get<std::vector<bool>>();
                    col->insert(col->end(), jsonCol.begin(), jsonCol.end());
                    break;
                }
                case nlohmann::detail::value_t::number_unsigned: {
                    auto* col = static_cast<Column<uint64_t>*>(result[i].get());
                    auto jsonCol = jsonData[i].get<std::vector<uint64_t>>();
                    col->insert(col->end(), jsonCol.begin(), jsonCol.end());
                    break;
                }
                case nlohmann::detail::value_t::number_integer: {
                    auto* col = static_cast<Column<int64_t>*>(result[i].get());
                    auto jsonCol = jsonData[i].get<std::vector<int64_t>>();
                    col->insert(col->end(), jsonCol.begin(), jsonCol.end());
                    break;
                }
                default:
                    break;
            };
        }

        return size * nmemb;
    };
    _client.sendRequest(req, func);
}
