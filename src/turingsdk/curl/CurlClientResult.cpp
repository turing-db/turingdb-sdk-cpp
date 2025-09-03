#include "CurlClientResult.h"

#include <curl/curl.h>

using namespace turingsdk;

std::string CurlClientError::fmtMessage() const {
    std::string curlClientErrorString = fmt::format("Curl Client Error : {}", CurlClientErrorTypeDescription::value(_type));

    if (_libCurlError) {
        curlClientErrorString += fmt::format(", Curl Error: {}", curl_easy_strerror(static_cast<CURLcode>(_libCurlError)));
    }

    if (_httpErrorCode) {
        curlClientErrorString += fmt::format(", HTTP Error Code:{}", _httpErrorCode);
    }

    return curlClientErrorString;
}
