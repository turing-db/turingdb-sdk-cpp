#pragma once

#include "BasicResult.h"
#include "CurlClientResult.h"
#include "EnumToString.h"

#include <spdlog/fmt/bundled/format.h>

namespace turingClient {

class Path;

enum class TuringRequestErrorType : uint8_t {
    UNKNOWN = 0,
    CANNOT_SEND_POST_REQUEST,
    TURING_QUERY_FAILED,
    UNKOWN_COLUMN_TYPE,
    CANNOT_LIST_AVAILABLE_GRAPHS,
    CANNOT_LIST_LOADED_GRAPHS,
    CANNOT_LOAD_GRAPH,
    _SIZE,
};

using TuringRequestErrorTypeDescription = EnumToString<TuringRequestErrorType>::Create<
    EnumStringPair<TuringRequestErrorType::UNKNOWN, "Unknown">,
    EnumStringPair<TuringRequestErrorType::CANNOT_SEND_POST_REQUEST, "Could not send the post request">,
    EnumStringPair<TuringRequestErrorType::TURING_QUERY_FAILED, "TuringDB Failed To Execute Query">,
    EnumStringPair<TuringRequestErrorType::UNKOWN_COLUMN_TYPE, "Type Of Result Column Is Unsupported">,
    EnumStringPair<TuringRequestErrorType::CANNOT_LIST_AVAILABLE_GRAPHS, "Could not list the available graphs">,
    EnumStringPair<TuringRequestErrorType::CANNOT_LIST_LOADED_GRAPHS, "Could not list the loaded graphs">,
    EnumStringPair<TuringRequestErrorType::CANNOT_LOAD_GRAPH, "Could not load graph">>;

class TuringRequestError {
public:
    TuringRequestError() = default;

    explicit TuringRequestError(TuringRequestErrorType type)
        : _type(type)
    {
    }

    TuringRequestError(TuringRequestErrorType type, CurlClientError curlError)
        : _type(type),
        _curlError(curlError)
    {
    }

    TuringRequestError(TuringRequestErrorType type, const std::string& errorMsg)
        : _type(type),
        _errorMsg(errorMsg)
    {
    }

    [[nodiscard]] TuringRequestErrorType getType() const { return _type; }
    [[nodiscard]] CurlClientError getCurlError() const { return _curlError; }
    [[nodiscard]] const std::string& getErrorMsg() const { return _errorMsg; }
    [[nodiscard]] std::string fmtMessage() const;

    static BadResult<TuringRequestError> result(TuringRequestErrorType type) {
        return BadResult<TuringRequestError>(TuringRequestError(type));
    }

    static BadResult<TuringRequestError> result(TuringRequestErrorType type, CurlClientError curlError) {
        return BadResult<TuringRequestError>(TuringRequestError(type, curlError));
    }

    static BadResult<TuringRequestError> result(TuringRequestErrorType type, const std::string& errorMsg) {
        return BadResult<TuringRequestError>(TuringRequestError(type, errorMsg));
    }

private:
    TuringRequestErrorType _type {};
    CurlClientError _curlError;
    std::string _errorMsg;
};

template <typename T>
using TuringRequestResult = BasicResult<T, class TuringRequestError>;

}
