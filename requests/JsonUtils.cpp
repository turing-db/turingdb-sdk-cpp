#include "JsonUtils.h"

#include "Profiler.h"
#include "TypedColumn.h"

namespace turingClient {

TuringRequestResult<void> checkJsonError(const json& jsonMsg, const TuringRequestErrorType retErrType) {
    if ((!jsonMsg.is_object())) {
        return TuringRequestError::result(TuringRequestErrorType::UNKNOWN_JSON_FORMAT);
    }

    if (jsonMsg.contains("error")) {
        const auto& errorMsg = jsonMsg["error"];
        if (!errorMsg.is_null()) {
            return TuringRequestError::result(retErrType, errorMsg.get<std::string>());
        }
    }
    return {};
}

TuringRequestResult<void> parseJson(char* location, std::vector<std::unique_ptr<TypedColumn>>& result) {
    Profile profile {"TuringRequest::parseJson"};

    if (!location || strlen(location) == 0) {
        return TuringRequestError::result(TuringRequestErrorType::UNKNOWN_JSON_FORMAT);
    }

    Profile* jsonParseProfile = new Profile("json Parsing");
    const json res = json::parse(location, nullptr, false);
    delete jsonParseProfile;

    if (res.is_discarded()) {
        return TuringRequestError::result(TuringRequestErrorType::INVALID_JSON_FORMAT);
    }

    if (auto ret = checkJsonError(res, turingClient::TuringRequestErrorType::TURING_QUERY_FAILED); !ret) {
        return ret;
    }

    if (!res.contains("header") || !res.contains("data") || !res["data"].is_array()) {
        return TuringRequestError::result(TuringRequestErrorType::UNKNOWN_JSON_FORMAT);
    }

    if (!res["header"].contains("column_names") || !res["header"].contains("column_types") || !res["header"]["column_names"].is_array() || !res["header"]["column_types"].is_array()) {
        return TuringRequestError::result(TuringRequestErrorType::UNKNOWN_JSON_FORMAT);
    }

    const auto& jsonHeader = res["header"];
    const auto colNames = jsonHeader["column_names"].get<std::vector<std::string>>();
    const auto colTypes = jsonHeader["column_types"].get<std::vector<std::string>>();
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
                return TuringRequestError::result(TuringRequestErrorType::UNKOWN_COLUMN_TYPE);
            }
        }
    }

    for (const auto& jsonData : res["data"]) {
        for (size_t i = 0; i < numCols; ++i) {
            switch (result[i].get()->columnType()) {
                case ColumnType::STRING: {
                    auto* col = static_cast<Column<std::string>*>(result[i].get());
                    for (const auto& val : jsonData[i]) {
                        if (!val.is_null()) {
                            col->push_back(val.get<std::string>());
                        } else {
                            col->pushNull();
                        }
                    }
                    break;
                }
                case ColumnType::BOOL: {
                    auto* col = static_cast<Column<CustomBool>*>(result[i].get());
                    for (const auto& val : jsonData[i]) {
                        if (!val.is_null()) {
                            col->push_back(val.get<bool>());
                        } else {
                            col->pushNull();
                        }
                    }
                    break;
                }
                case ColumnType::UINT: {
                    auto* col = static_cast<Column<uint64_t>*>(result[i].get());
                    for (const auto& val : jsonData[i]) {
                        if (!val.is_null()) {
                            col->push_back(val.get<uint64_t>());
                        } else {
                            col->pushNull();
                        }
                    }
                    break;
                }
                case ColumnType::INT: {
                    auto* col = static_cast<Column<int64_t>*>(result[i].get());
                    for (const auto& val : jsonData[i]) {
                        if (!val.is_null()) {
                            col->push_back(val.get<int64_t>());
                        } else {
                            col->pushNull();
                        }
                    }
                    break;
                }
                default:
                    break;
            };
        }
    }

    return {};
}

}
