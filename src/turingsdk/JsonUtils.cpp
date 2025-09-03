#include "JsonUtils.h"

#include "base/TypedColumn.h"

using namespace turingsdk;

TuringClientResult JsonUtils::parseJson(char* str,
                                        std::vector<std::unique_ptr<TypedColumn>>& result) {
    if (!str || !*str) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    const auto jsonRes = nlohmann::json::parse(str, nullptr, false);

    if (jsonRes.is_discarded()) {
        return TuringClientError::result(TuringClientErrorType::INVALID_JSON_FORMAT);
    }

    if (auto ret = checkJsonError(jsonRes, turingsdk::TuringClientErrorType::TURING_QUERY_FAILED); !ret) {
        return ret;
    }

    // Read header
    const auto headerIt = jsonRes.find("header");
    if (headerIt == jsonRes.end()) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    // Search for column names and types
    const auto colNamesIt = headerIt->find("column_names");
    const auto colTypesIt = headerIt->find("column_types");
    if (colNamesIt == headerIt->end()
        || colTypesIt == headerIt->end()
        || !((*colNamesIt).is_array())
        || !((*colTypesIt).is_array())) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    // Search for data
    const auto dataIt = jsonRes.find("data");
    if (dataIt == jsonRes.end()) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    if (!((*dataIt).is_array())) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    const std::vector<std::string>& colNames = (*colNamesIt).get<std::vector<std::string>>();
    const std::vector<std::string>& colTypes = (*colTypesIt).get<std::vector<std::string>>();
    const size_t numCols = colNames.size();

    // Create columns
    if (result.empty()) {
        for (size_t i = 0; i < numCols; ++i) {
            if (colTypes[i] == "String") {
                result.emplace_back(std::make_unique<Column<std::string>>(colNames[i]));
            } else if (colTypes[i] == "Bool") {
                result.emplace_back(std::make_unique<Column<CustomBool>>(colNames[i]));
            } else if (colTypes[i] == "UInt64") {
                result.emplace_back(std::make_unique<Column<uint64_t>>(colNames[i]));
            } else if (colTypes[i] == "Int64") {
                result.emplace_back(std::make_unique<Column<int64_t>>(colNames[i]));
            } else {
                return TuringClientError::result(TuringClientErrorType::UNKOWN_COLUMN_TYPE);
            }
        }
    }

    // Add data to columns lambda
    const auto addData = [&]<SupportedValueType T>(size_t i, const auto& data) {
        using ActualType = std::conditional_t<isCustomBool<T>, bool, T>;
        auto* col = static_cast<Column<T>*>(result[i].get());
        for (const auto& val : data[i]) {
            if (!val.is_null()) {
                col->push_back(val.template get<ActualType>());
            } else {
                col->pushNull();
            }
        }
    };

    // Add data to columns
    for (const auto& data : (*dataIt)) {
        for (size_t i = 0; i < numCols; ++i) {
            TypeDispatcher {result[i].get()->columnType()}.execute(addData, i, data);
        }
    }

    return {};
}

TuringClientResult JsonUtils::checkJsonError(const nlohmann::json& jsonMsg,
                                             const TuringClientErrorType& retErrType) {
    if (!jsonMsg.is_object()) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    const auto jsonIt = jsonMsg.find("error");
    if (jsonIt != jsonMsg.end()) {
        const auto& errorMsg = jsonMsg["error"];
        if (!(*jsonIt).is_null()) {
            return TuringClientError::result(retErrType, errorMsg.get<std::string>());
        }
    }

    return {};
}