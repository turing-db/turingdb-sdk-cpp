#pragma once

#include <string>
#include <vector>

#include "CurlClientResult.h"

namespace turingsdk {

class CurlClient {
public:
    using Buffer = std::vector<char>;

    CurlClient();
    ~CurlClient();

    Buffer& getBuffer();

    void addHeader(const std::string& headerKey, const std::string& headerValue);
    void setUrl(const std::string& url);

    void setPostData(const std::string& data);
    void clearPostData();

    CurlClientResult send();

private:
    void* _curl {nullptr};
    Buffer _buffer;
};

}
