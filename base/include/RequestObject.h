#pragma once

#include <cstdint>
#include <string_view>
#include <string>

enum HTTP_METHOD : uint8_t {
    UNKOWN = 0,
    POST,
    GET,
    PUT
};

struct RequestObject {
    HTTP_METHOD method;
    std::string_view url;
    std::string_view endpoint;
    std::string body;
};
