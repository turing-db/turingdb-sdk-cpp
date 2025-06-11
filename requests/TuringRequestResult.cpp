#include "TuringRequestResult.h"

#include <curl/curl.h>

using namespace turingClient;

std::string TuringRequestError::fmtMessage() const {
    std::string turingRequestErrorString = fmt::format("Turing Request Error: {}", TuringRequestErrorTypeDescription::value(_type));

    if (_curlError.getType() != CurlClientErrorType::UNKNOWN) {
        return fmt::format("{}, {}", turingRequestErrorString,
                           _curlError.fmtMessage());
    }
    if (!_errorMsg.empty()) {
        return fmt::format("{}, TuringDB Error: {}", turingRequestErrorString,
                           _errorMsg);
    }

    return turingRequestErrorString;
}
