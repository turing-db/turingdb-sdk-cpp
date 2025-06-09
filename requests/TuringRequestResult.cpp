#include "CurlClientResult.h"
#include <curl/curl.h>

using namespace turingClient;

std::string CurlClientError::fmtMessage() const {

    return fmt::format("Turing Request Error: {}",
                       ErrorTypeDescription::value(_type));
}
