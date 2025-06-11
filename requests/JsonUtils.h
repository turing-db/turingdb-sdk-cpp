#pragma once
#include <nlohmann/json.hpp>

#include "TuringRequestResult.h"

using json = nlohmann::json;

namespace turingClient {

class TypedColumn;

TuringRequestResult<void> checkJsonError(const json& jsonMsg, const TuringRequestErrorType retErrType);
TuringRequestResult<void> parseJson(char* location, std::vector<std::unique_ptr<TypedColumn>>& result);
}
