#include <iostream>
#include <curl/curl.h>
#include <memory>

#include "TuringClient.h"

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
    std::string url = "http://127.0.0.1:8080";
    turingClient::TuringClient request(url);
    request.setBearerToken("vwxy345efg678hij901klm234nop567pqr890stu123vwx456");
    std::string graph = "simpledb";
    std::string graphFake = "simplestdb";

    if (auto res = request.loadGraph(graph); !res) {
        std::cout << request.getError().fmtMessage() << std::endl;
    }
    if (auto res = request.loadGraph(graphFake); !res) {
        std::cout << request.getError().fmtMessage() << std::endl;
    }
    if (auto res = request.loadGraph(graph); !res) {
        std::cout << request.getError().fmtMessage() << std::endl;
    }

    std::string query = "match n-[e]-m return n,n.name,e.duration, m.hasPhD";
    std::vector<std::unique_ptr<TypedColumn>> result;
    request.query(query, graph, result);

    printTypedColumns(result);
}
