#pragma once

#include <spdlog/fmt/bundled/format.h>

#include "BasicResult.h"
#include "EnumToString.h"
#include "CurlClientResult.h"

namespace turingClient {

class Path;

enum class TuringClientErrorType : uint8_t {
  UNKNOWN = 0,
  CANNOT_SEND_POST_REQUEST,
  TURING_QUERY_FAILED,
  UNKNOWN_JSON_FORMAT,
  INVALID_JSON_FORMAT,
  UNKOWN_COLUMN_TYPE,
  CANNOT_SET_BEARER_TOKEN,
  CANNOT_LIST_AVAILABLE_GRAPHS,
  CANNOT_LIST_LOADED_GRAPHS,
  CANNOT_QUERY_GRAPH,
  CANNOT_LOAD_GRAPH,
  CANNOT_LOAD_HISTORY,
  _SIZE,
};

using TuringClientErrorTypeDescription =
    EnumToString<TuringClientErrorType>::Create<
        EnumStringPair<TuringClientErrorType::UNKNOWN, "Unknown">,
        EnumStringPair<TuringClientErrorType::CANNOT_SEND_POST_REQUEST,
                       "Could not send the post request">,
        EnumStringPair<TuringClientErrorType::TURING_QUERY_FAILED,
                       "TuringDB Failed To Execute Query">,
        EnumStringPair<TuringClientErrorType::UNKNOWN_JSON_FORMAT,
                       "Could not parse unknown json format">,
        EnumStringPair<TuringClientErrorType::INVALID_JSON_FORMAT,
                       "Could not parse invalid json format">,
        EnumStringPair<TuringClientErrorType::UNKOWN_COLUMN_TYPE,
                       "Type Of Result Column Is Unsupported">,
        EnumStringPair<TuringClientErrorType::CANNOT_SET_BEARER_TOKEN,
                       "Could not set the bearer token">,
        EnumStringPair<TuringClientErrorType::CANNOT_LIST_AVAILABLE_GRAPHS,
                       "Could not list the available graphs">,
        EnumStringPair<TuringClientErrorType::CANNOT_LIST_LOADED_GRAPHS,
                       "Could not list the loaded graphs">,
        EnumStringPair<TuringClientErrorType::CANNOT_QUERY_GRAPH,
                       "Could not send query to the graph">,
        EnumStringPair<TuringClientErrorType::CANNOT_LOAD_GRAPH,
                       "Could not load graph">,
        EnumStringPair<TuringClientErrorType::CANNOT_LOAD_HISTORY,
                       "Could not load history">>;

class TuringClientError {
public:
  TuringClientError() = default;

  explicit TuringClientError(TuringClientErrorType type) : _type(type) {}

  TuringClientError(TuringClientErrorType type, CurlClientError curlError)
      : _type(type), _curlError(curlError) {}

  TuringClientError(TuringClientErrorType type, const std::string &errorMsg)
      : _type(type), _errorMsg(errorMsg) {}

  [[nodiscard]] TuringClientErrorType getType() const { return _type; }
  [[nodiscard]] CurlClientError getCurlError() const { return _curlError; }
  [[nodiscard]] const std::string &getErrorMsg() const { return _errorMsg; }
  [[nodiscard]] std::string fmtMessage() const;

  static BadResult<TuringClientError> result(TuringClientErrorType type) {
    return BadResult<TuringClientError>(TuringClientError(type));
  }

  static BadResult<TuringClientError> result(TuringClientErrorType type,
                                             CurlClientError curlError) {
    return BadResult<TuringClientError>(TuringClientError(type, curlError));
  }

  static BadResult<TuringClientError> result(TuringClientErrorType type,
                                             const std::string &errorMsg) {
    return BadResult<TuringClientError>(TuringClientError(type, errorMsg));
  }

private:
    TuringClientErrorType _type {};
    CurlClientError _curlError;
    std::string _errorMsg;
};

template <typename T>
using TuringClientResult = BasicResult<T, class TuringClientError>;

}
