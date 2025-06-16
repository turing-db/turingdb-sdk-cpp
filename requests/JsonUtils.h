#pragma once
#include <nlohmann/json.hpp>

#include "TuringClientResult.h"

using json = nlohmann::json;

namespace turingClient {

class TypedColumn;

TuringClientResult<void> checkJsonError(const json& jsonMsg,
                                        const TuringClientErrorType& retErrType);
TuringClientResult<void> parseJson(char* location,
                                   std::vector<std::unique_ptr<TypedColumn>>& result);
}
