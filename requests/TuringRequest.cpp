#include "TuringRequest.h"

#include "RequestObject.h"
#include "CurlClient.h"
#include <iostream>

using namespace turingClient;

void TuringRequest::listAvailableGraphs(WriteCallBack func) {
    RequestObject req = {HTTP_METHOD::POST, "http://127.0.0.1:6666", "/list_avail_graphs"};

    _client.sendRequest(req, func);
}

void TuringRequest::listLoadedGraphs(WriteCallBack func) {
    RequestObject req = {HTTP_METHOD::POST, "127.0.0.1:6666", "/list_loaded_graphs"};

    _client.sendRequest(req, func);
}
