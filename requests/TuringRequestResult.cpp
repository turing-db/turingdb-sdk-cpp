#include "TuringRequestResult.h"
#include <curl/curl.h>

using namespace turingClient;

std::string TuringRequestError::fmtMessage() const {
    return fmt::format("Turing Request Error: {}",
                       TuringRequestErrorTypeDescription::value(_type));
}
