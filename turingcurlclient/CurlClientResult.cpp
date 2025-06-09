#include "CurlClientResult.h"
#include <curl/curl.h>

using namespace turingClient;

std::string CurlClientError::fmtMessage() const {
    return fmt::format("Turing CurlClient error: {}, Curl Error {}",
                       ErrorTypeDescription::value(_type), curl_easy_strerror(static_cast<CURLcode>(_libCurlError)));
}
