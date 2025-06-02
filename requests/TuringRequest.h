#pragma once

#include "CurlClient.h"
#include "CallBackSignatures.h"
#include "TypedColumn.h"
#include <memory>


namespace turingClient {

class TuringRequest {

public:
    explicit TuringRequest(std::string&);
    ~TuringRequest() = default;

    void listLoadedGraphs(std::vector<std::string>& result);
    void listAvailableGraphs(std::vector<std::string>& result);
    void loadGraph(std::string_view graph);
    void query(std::string& query, std::string& graph, std::vector<std::unique_ptr<TypedColumn>>& result);

private:
    CurlClient _client;
    std::string& _url;
    std::vector<char> _buffer;
};

}
