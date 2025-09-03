#pragma once

#include <nlohmann/json.hpp>

#include "TuringClientResult.h"

namespace turingsdk {

class TypedColumn;

class JsonUtils {
public:
    static TuringClientResult parseJson(char* str,
                          std::vector<std::unique_ptr<TypedColumn>>& result);

    static TuringClientResult checkJsonError(const nlohmann::json& json,
                                             const TuringClientErrorType& retErrType);

    JsonUtils() = delete;
};

}
