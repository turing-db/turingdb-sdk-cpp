#include <iostream>
#include <curl/curl.h>
#include <memory>

#include "TuringRequest.h"

using namespace turingClient;

void printTypedColumns(std::vector<std::unique_ptr<TypedColumn>>& cols) {
    for (size_t i = 0; i < cols.size(); ++i) {
        switch (cols[i]->columnType()) {
            case INT:
                static_cast<Column<int64_t>*>(cols[i].get())->dump();
                break;
            case UINT:
                static_cast<Column<uint64_t>*>(cols[i].get())->dump();
                break;
            case STRING:
                static_cast<Column<std::string>*>(cols[i].get())->dump();
                break;
            case BOOL:
                static_cast<Column<CustomBool>*>(cols[i].get())->dump();
                break;
            default:
                break;
        }
    }
}

int main() {
    std::string url = "http://127.0.0.1:6666";
    turingClient::TuringRequest request(url);
    std::string graph = "simpledb";
    if (auto res = request.loadGraph(graph); !res) {
        std::cout << res.error().fmtMessage() << std::endl;
    }

    // std::vector<std::string> res;
    // if(auto n = request.listLoadedGraphs(res); !n){
    //     std::cout<<n.error().fmtMessage()<<std::endl;
    //     return 0;
    // }
    // for (const auto& graph : res) {
    //     std::cout << graph << std::endl;
    // }



    std::string query = "match n-[e]-m return n,n.name,e.duration, m.hasPhD";
    std::vector<std::unique_ptr<TypedColumn>> result;
    request.query(query, graph, result);

    // printTypedColumns(result);
}
