#pragma once

#include <functional>
#include <string>

#include "CallBackSignatures.h"


namespace turingClient {

class CurlRequest {
public:
    WriteCallBack callBackFn;

    CurlRequest();
    ~CurlRequest();

    int setUrl(std::string& url);
    int setPost();
    int setWriteCallBack(WriteCallBack func);
    int send();

private:
    void* _handle;
};

}
