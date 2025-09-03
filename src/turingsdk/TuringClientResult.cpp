#include "TuringClientResult.h"

#include <spdlog/fmt/bundled/format.h>

using namespace turingsdk;

std::string TuringClientError::fmtMessage() const {
    std::string turingRequestErrorString = fmt::format("Turing Request Error: {}", TuringClientErrorTypeDescription::value(_type));

    if (_curlError.getType() != CurlClientErrorType::UNKNOWN) {
        turingRequestErrorString += fmt::format(", {}", _curlError.fmtMessage());
    } else if (!_errorMsg.empty()) {
        turingRequestErrorString += fmt::format(", TuringDB Error: {}", _errorMsg);
    }

    return turingRequestErrorString;
}
