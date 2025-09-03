#include "TuringClientResult.h"

#include <curl/curl.h>

using namespace turingClient;

std::string TuringClientError::fmtMessage() const {
    std::string turingRequestErrorString = fmt::format("Turing Request Error: {}", TuringClientErrorTypeDescription::value(_type));

    if (_curlError.getType() != CurlClientErrorType::UNKNOWN) {
        turingRequestErrorString += fmt::format(", {}", _curlError.fmtMessage());
    } else if (!_errorMsg.empty()) {
        turingRequestErrorString += fmt::format(", TuringDB Error: {}", _errorMsg);
    }

    return turingRequestErrorString;
}
