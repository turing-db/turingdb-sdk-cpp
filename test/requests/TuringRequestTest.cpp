#include "gtest/gtest.h"

#include "turingsdk/base/TypedColumn.h"
#include "turingsdk/JsonUtils.h"

using namespace turingClient;

class TuringClientTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {}
};

bool compareColumns(TypedColumn* typedCol1, TypedColumn* typedCol2) {
    switch (typedCol1->columnType()) {
        case INT:
            return *(static_cast<Column<int64_t>*>(typedCol1)) == *(static_cast<Column<int64_t>*>(typedCol2));
        case UINT:
            return *(static_cast<Column<uint64_t>*>(typedCol1)) == *(static_cast<Column<uint64_t>*>(typedCol2));
        case STRING:
            return *(static_cast<Column<std::string>*>(typedCol1)) == *(static_cast<Column<std::string>*>(typedCol2));
        case BOOL:
            return *(static_cast<Column<CustomBool>*>(typedCol1)) == *(static_cast<Column<CustomBool>*>(typedCol2));
        default:
            return false;
    }
}

TEST_F(TuringClientTest, jsonValidParseTests) {
    std::string jsonResult = "{\"header\":{\"column_names\":[\"n\",\"n.name\",\"e.duration\",\"m.hasPhD\"],\"column_types\":[\"UInt64\",\"String\",\"Int64\",\"Bool\"]},\"data\":[[[0,0,0,0,1,1,1,6,8,8,9,9,11],[\"Remy\",\"Remy\",\"Remy\",\"Remy\",\"Adam\",\"Adam\",\"Adam\",\"Ghosts\",\"Maxime\",\"Maxime\",\"Luc\",\"Luc\",\"Martina\"],[20,20,null,20,20,null,null,200,null,null,20,15,10],[true,null,null,null,true,null,null,true,null,null,null,null,null]]],\"time\":0.206431}";

    std::vector<TypedColumn*> expected;
    Column<uint64_t> col1 = {0, 0, 0, 0, 1, 1, 1, 6, 8, 8, 9, 9, 11};
    Column<std::string> col2 = {"Remy", "Remy", "Remy", "Remy", "Adam", "Adam", "Adam", "Ghosts", "Maxime", "Maxime", "Luc", "Luc", "Martina"};
    Column<int64_t> col3 = {20, 20, std::nullopt, 20, 20, std::nullopt, std::nullopt, 200, std::nullopt, std::nullopt, 20, 15, 10};
    Column<CustomBool> col4 = {true, std::nullopt, std::nullopt, std::nullopt, true, std::nullopt, std::nullopt, true, std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt};
    expected.push_back(&col1);
    expected.push_back(&col2);
    expected.push_back(&col3);
    expected.push_back(&col4);

    std::vector<std::unique_ptr<TypedColumn>> cols;
    auto res = parseJson(jsonResult.data(), cols);
    ASSERT_TRUE(res);

    ASSERT_EQ(cols.size(), 4);
    ASSERT_EQ(cols[0]->columnType(), ColumnType::UINT);
    ASSERT_EQ(cols[1]->columnType(), ColumnType::STRING);
    ASSERT_EQ(cols[2]->columnType(), ColumnType::INT);
    ASSERT_EQ(cols[3]->columnType(), ColumnType::BOOL);

    for (size_t i = 0; i < cols.size(); ++i) {
        ASSERT_TRUE(compareColumns(cols[i].get(), expected[i]));
    }
}
TEST_F(TuringClientTest, jsonInvalidFormatTest) {
    std::string jsonBadFormat = "{\"header\":{\"col_names\":[\"n\",\"n.name\",\"e.duration\",\"m.hasPhD\"],\"column_types\":[\"UInt64\",\"String\",\"Int64\",\"Bool\"]},\"data\":[[[0,0,0,0,1,1,1,6,8,8,9,9,11],[\"Remy\",\"Remy\",\"Remy\",\"Remy\",\"Adam\",\"Adam\",\"Adam\",\"Ghosts\",\"Maxime\",\"Maxime\",\"Luc\",\"Luc\",\"Martina\"],[20,20,null,20,20,null,null,200,null,null,20,15,10],[true,null,null,null,true,null,null,true,null,null,null,null,null]]],\"time\":0.206431}";

    std::vector<std::unique_ptr<TypedColumn>> cols;
    auto res = parseJson(jsonBadFormat.data(), cols);
    ASSERT_FALSE(res);
    ASSERT_EQ(res.error().getType(), TuringClientErrorType::UNKNOWN_JSON_FORMAT);
}

TEST_F(TuringClientTest, jsonEmptyStringTest) {
    std::string jsonEmptyString;
    std::vector<std::unique_ptr<TypedColumn>> cols;

    auto res = parseJson(jsonEmptyString.data(), cols);
    ASSERT_FALSE(res);
    ASSERT_EQ(res.error().getType(), TuringClientErrorType::UNKNOWN_JSON_FORMAT);
}

TEST_F(TuringClientTest, jsonEmptyObjectTest) {
    std::string jsonEmptyObject = "{}";
    std::vector<std::unique_ptr<TypedColumn>> cols;

    auto res = parseJson(jsonEmptyObject.data(), cols);
    ASSERT_FALSE(res);
    ASSERT_EQ(res.error().getType(), TuringClientErrorType::UNKNOWN_JSON_FORMAT);
}

TEST_F(TuringClientTest, jsonNoArrayTest) {
    std::string jsonNoArray = "{\"key\":0}";
    std::vector<std::unique_ptr<TypedColumn>> cols;

    auto res = parseJson(jsonNoArray.data(), cols);
    ASSERT_FALSE(res);
    ASSERT_EQ(res.error().getType(), TuringClientErrorType::UNKNOWN_JSON_FORMAT);
}


TEST_F(TuringClientTest, jsonInvalidColumnType) {
    std::string jsonInvalidColumnType = "{\"header\":{\"column_names\":[\"n\",\"n.name\",\"e.duration\",\"m.hasPhD\"],\"column_types\":[\"UInt64\",\"String\",\"Float\",\"Bool\"]},\"data\":[[[0,0,0,0,1,1,1,6,8,8,9,9,11],[\"Remy\",\"Remy\",\"Remy\",\"Remy\",\"Adam\",\"Adam\",\"Adam\",\"Ghosts\",\"Maxime\",\"Maxime\",\"Luc\",\"Luc\",\"Martina\"],[20,20,null,20,20,null,null,200,null,null,20,15,10],[true,null,null,null,true,null,null,true,null,null,null,null,null]]],\"time\":0.206431}";


    std::vector<std::unique_ptr<TypedColumn>> cols;
    auto res = parseJson(jsonInvalidColumnType.data(), cols);
    ASSERT_FALSE(res);
    ASSERT_EQ(res.error().getType(), TuringClientErrorType::UNKOWN_COLUMN_TYPE);
}

TEST_F(TuringClientTest, jsonErrorTest) {
    std::string jsonNoArray = "{\"error\":\"PLAN ERROR\"}";
    std::vector<std::unique_ptr<TypedColumn>> cols;

    auto res = parseJson(jsonNoArray.data(), cols);
    ASSERT_FALSE(res);
    ASSERT_EQ(res.error().getType(), TuringClientErrorType::TURING_QUERY_FAILED);
    ASSERT_EQ(res.error().getErrorMsg(), "PLAN ERROR");
}
