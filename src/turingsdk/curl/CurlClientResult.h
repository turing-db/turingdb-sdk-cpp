#pragma once

#include "BasicResult.h"
#include "EnumToString.h"

namespace turingsdk {

enum class CurlClientErrorType : uint8_t {
    UNKNOWN = 0,
    CANNOT_SET_POST_REQUEST,
    CANNOT_SET_HEADERS,
    CANNOT_SET_URL,
    CANNOT_SET_BEARER_TOKEN,
    CANNOT_SET_WRITE_CALLBACK,
    CANNOT_SEND_REQUEST,
    HTTP_ERROR,
    UNKNOWN_HTTP_METHOD,
    _SIZE,
};

using CurlClientErrorTypeDescription = EnumToString<CurlClientErrorType>::Create<
    EnumStringPair<CurlClientErrorType::UNKNOWN, "Unknown">,
    EnumStringPair<CurlClientErrorType::CANNOT_SET_POST_REQUEST, "Could not set the curl request to POST">,
    EnumStringPair<CurlClientErrorType::CANNOT_SET_HEADERS, "Could not set the HTTP headeres">,
    EnumStringPair<CurlClientErrorType::CANNOT_SET_URL, "Could not set the destination URL on the request">,
    EnumStringPair<CurlClientErrorType::CANNOT_SET_BEARER_TOKEN, "Could not set the beaerer token">,
    EnumStringPair<CurlClientErrorType::CANNOT_SET_WRITE_CALLBACK, "Could not set the write callback function">,
    EnumStringPair<CurlClientErrorType::CANNOT_SEND_REQUEST, "Could not send the CURL request">,
    EnumStringPair<CurlClientErrorType::HTTP_ERROR, "Received HTTP Error Code">,
    EnumStringPair<CurlClientErrorType::UNKNOWN_HTTP_METHOD, "HTTP Method Unsuported By Turing Curl Client">>;

class CurlClientError {
public:
    CurlClientError() = default;
    CurlClientError(CurlClientErrorType type) 
        : _type(type)
    {
    }

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
    static BadResult<CurlClientError> result(CurlClientErrorType type) {
        return BadResult<CurlClientError>(CurlClientError(type));
    }

    template <typename... T>
    static BadResult<CurlClientError> result(CurlClientErrorType type,
                                             int libCurlError) {
        return BadResult<CurlClientError>(CurlClientError(type, libCurlError));
    }

    template <typename... T>
    static BadResult<CurlClientError> result(CurlClientErrorType type,
                                             long httpErrorCode) {
        return BadResult<CurlClientError>(CurlClientError(type, httpErrorCode));
    }

private:
    CurlClientErrorType _type {};
    int _libCurlError {0};
    long _httpErrorCode {0};
};

using CurlClientResult = BasicResult<void, class CurlClientError>;

}
