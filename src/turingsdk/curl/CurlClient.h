#pragma once

#include <string>
#include <unordered_map>

#include "CurlClientResult.h"

namespace turingsdk {

class CurlClient {
public:
    using Buffer = std::vector<char>;

    CurlClient();
    ~CurlClient();

    Buffer& getBuffer() { return _buffer; }

    void setHeader(const std::string& headerKey, const std::string& headerValue);
    void setUrl(const std::string& url);

    void setPostData(const std::string& data);
    void clearPostData();

    CurlClientResult send();

private:
    void* _curl {nullptr};
    std::unordered_map<std::string, std::string> _headers;
    Buffer _buffer;
};

}
