#include "CurlClientResult.h"
#include <curl/curl.h>

using namespace turingClient;

std::string CurlClientError::fmtMessage() const {
    std::string curlClientErrorString = fmt::format("Curl Client Error : {}", CurlClientErrorTypeDescription::value(_type));

    if (_libCurlError) {
        return fmt::format("{}, Curl Error: {}", curlClientErrorString, curl_easy_strerror(static_cast<CURLcode>(_libCurlError)));
    }

    if (_httpErrorCode) {
        return fmt::format("{}, HTTP Error Code:{}", curlClientErrorString, _httpErrorCode);
    }

    return std::string(curlClientErrorString);
}
