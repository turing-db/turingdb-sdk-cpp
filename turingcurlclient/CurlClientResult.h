#pragma once

#include "BasicResult.h"
#include "EnumToString.h"

#include <spdlog/fmt/bundled/format.h>

namespace turingClient {

class Path;

enum class CurlClientErrorType : uint8_t {
    UNKNOWN = 0,
    CANNOT_SET_POST_REQUEST,
    CANNOT_SET_URL_REQUEST,
    CANNOT_SET_WRITE_CALLBACK,
    CANNOT_SEND_REQUEST,
    HTTP_ERROR,
    UNKNOWN_HTTP_METHOD,
    _SIZE,
};

using CurlClientErrorTypeDescription = EnumToString<CurlClientErrorType>::Create<
    EnumStringPair<CurlClientErrorType::UNKNOWN, "Unknown">,
    EnumStringPair<CurlClientErrorType::CANNOT_SET_POST_REQUEST, "Could not set the curl request to POST">,
    EnumStringPair<CurlClientErrorType::CANNOT_SET_URL_REQUEST, "Could not set the destination URL on the request">,
    EnumStringPair<CurlClientErrorType::CANNOT_SET_WRITE_CALLBACK, "Could not set the write callback function">,
    EnumStringPair<CurlClientErrorType::CANNOT_SEND_REQUEST, "Could not send the CURL request">,
    EnumStringPair<CurlClientErrorType::HTTP_ERROR, "Received HTTP Error Code">,
    EnumStringPair<CurlClientErrorType::UNKNOWN_HTTP_METHOD, "HTTP Method Unsuported By Turing Curl Client">>;

class CurlClientError {
public:
    CurlClientError() = default;
    CurlClientError(CurlClientErrorType type,
                    const int libCurlError)
        : _type(type),
          _libCurlError(libCurlError)
    {
    }
    CurlClientError(CurlClientErrorType type,
                    const long httpErrorCode)
        : _type(type),
          _httpErrorCode(httpErrorCode)
    {
    }

    [[nodiscard]] CurlClientErrorType getType() const { return _type; }
    [[nodiscard]] int getCurlError() const { return _libCurlError; }
    [[nodiscard]] std::string fmtMessage() const;

    template <typename... T>
    static BadResult<CurlClientError> result(CurlClientErrorType type,
                                             int libCurlError = 0) {
        return BadResult<CurlClientError>(CurlClientError(type, libCurlError));
    }

private:
    CurlClientErrorType _type {};
    int _libCurlError {0};
    long _httpErrorCode {0};
};

template <typename T>
using CurlClientResult = BasicResult<T, class CurlClientError>;
}
