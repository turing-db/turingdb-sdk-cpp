#include <iostream>
#include <curl/curl.h>
#include <memory>

#include "TuringRequest.h"

using namespace turingClient;

void printTypedColumns(std::vector<std::unique_ptr<TypedColumn>>& cols) {
    for (size_t i = 0; i < cols.size(); ++i) {
        switch (cols[i]->column_type()) {
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
    turingClient::TuringRequest request;
    std::vector<std::string> res;
    // request.listAvailableGraphs(res);
    // for(const auto& graph: res){
    //     std::cout<<graph<<std::endl;

    //}
    std::string query = "match n-[e]-m return n,e.name,m.name";
    std::string graph = "simpledb";
    std::vector<std::unique_ptr<TypedColumn>> result;
    request.query(query, graph, result);

    printTypedColumns(result);
}
