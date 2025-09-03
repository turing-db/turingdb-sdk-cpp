#include "JsonUtils.h"

#include "base/TypedColumn.h"

using namespace turingsdk;

TuringClientResult<void>
turingsdk::checkJsonError(const json& jsonMsg,
                          const TuringClientErrorType& retErrType) {
    if ((!jsonMsg.is_object())) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    if (const auto jsonIt = jsonMsg.find("error"); jsonIt != jsonMsg.end()) {
        const auto& errorMsg = jsonMsg["error"];
        if (!(*jsonIt).is_null()) {
            return TuringClientError::result(retErrType, errorMsg.get<std::string>());
        }
    }
    return {};
}

TuringClientResult<void>
turingsdk::parseJson(char* location,
                     std::vector<std::unique_ptr<TypedColumn>>& result) {
    if (!location || !*location) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    const json res = json::parse(location, nullptr, false);

    if (res.is_discarded()) {
        return TuringClientError::result(TuringClientErrorType::INVALID_JSON_FORMAT);
    }

    if (auto ret = checkJsonError(res, turingsdk::TuringClientErrorType::TURING_QUERY_FAILED); !ret) {
        return ret;
    }

    const auto headerIt = res.find("header");
    if (headerIt == res.end()) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    const auto colNamesIt = headerIt->find("column_names");
    const auto colTypesIt = headerIt->find("column_types");
    if (colNamesIt == headerIt->end()
        || colTypesIt == headerIt->end()
        || !((*colNamesIt).is_array())
        || !((*colTypesIt).is_array())) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    const auto dataIt = res.find("data");
    if (dataIt == res.end()) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    if (!((*dataIt).is_array())) {
        return TuringClientError::result(TuringClientErrorType::UNKNOWN_JSON_FORMAT);
    }

    const auto colNames = (*colNamesIt).get<std::vector<std::string>>();
    const auto colTypes = (*colTypesIt).get<std::vector<std::string>>();
    const auto numCols = colNames.size();

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

    for (const auto& data : (*dataIt)) {
        for (size_t i = 0; i < numCols; ++i) {
            TypeDispatcher {result[i].get()->columnType()}.execute(addData, i, data);
        }
    }

    return {};
}
