#pragma once

#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace turingClient {

struct CustomBool {
    CustomBool() = default;
    CustomBool(bool v)
        : _boolean(v)
    {
    }

    CustomBool& operator=(bool v) {
        _boolean = v;
        return *this;
    }
    operator bool() const { return _boolean; }

    bool operator==(const CustomBool& other) const {
        return _boolean == other._boolean;
    }

    bool _boolean {false};
};

enum ColumnType : uint8_t {
    INT,
    UINT,
    DOUBLE,
    STRING,
    BOOL
    // Add more types as needed
};

template <typename T>
concept SupportedColumnType = std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t> || std::is_same_v<T, double> || std::is_same_v<T, std::string> || std::is_same_v<T, CustomBool>;

class TypedColumn {
public:
    virtual ~TypedColumn() = default;
    virtual std::size_t size() const = 0;
    virtual const void* data() const = 0;
    virtual void* data() = 0;
    virtual const void* mask() const = 0;
    virtual void* mask() = 0;

    virtual ColumnType columnType() const = 0;
};

template <SupportedColumnType ColType>
class Column : public TypedColumn {
private:
    std::vector<ColType> _data;
    std::vector<int8_t> _mask;
    std::string _colName;

    // Map C++ types to enum values
    static constexpr ColumnType getColumnType() {
        if constexpr (std::is_same_v<ColType, int64_t>) {
            return ColumnType::INT;
        } else if constexpr (std::is_same_v<ColType, uint64_t>) {
            return ColumnType::UINT;
        } else if constexpr (std::is_same_v<ColType, double>) {
            return ColumnType::DOUBLE;
        } else if constexpr (std::is_same_v<ColType, std::string>) {
            return ColumnType::STRING;
        } else if constexpr (std::is_same_v<ColType, CustomBool>) {
            return ColumnType::BOOL;
        }
    }

public:
    Column() = default;

    explicit Column(std::string colName)
    :_colName(std::move(colName))
    {
    }

    Column(std::initializer_list<std::optional<ColType>> col)
        requires(!std::same_as<ColType, CustomBool>)
    {
        _data.reserve(col.size());
        _mask.reserve(col.size());

        for (const auto& val : col) {
            if (val.has_value()) {
                _data.push_back(val.value());
                _mask.push_back(0);
            } else {
                _data.push_back(ColType {});
                _mask.push_back(1);
            }
        }
    }

    Column(std::initializer_list<std::optional<bool>> col)
        requires std::same_as<ColType, CustomBool>
    {
        _data.reserve(col.size());

        for (const auto& val : col) {
            if (val.has_value()) {
                _data.push_back(CustomBool(val.value()));
                _mask.push_back(0);
            } else {
                _data.push_back(CustomBool {});
                _mask.push_back(1);
            }
        }
    }

    std::strong_ordering operator<=>(const Column& other) const {
        if (auto cmp = _data <=> other._data(); cmp != std::strong_ordering::equal) {
            return cmp;
        }
        return _mask <=> other._mask;
    }

    bool operator==(const Column& other) const {
        return _data == other._data && _mask == other._mask;
    }


    ColumnType columnType() const override {
        return getColumnType();
    }

    std::string& getColumnName() {
        return _colName;
    }

    const std::vector<ColType>& get() const { return _data; }

    // MaskOps
    const std::vector<int8_t>& getMask() const { return _mask; }

    void pushDataAndMask(const ColType& data, const int8_t mask) {
        _data.push_back(data);
        _mask.push_back(mask);
    }

    void push_back(const ColType& data) {
        pushDataAndMask(data, 0);
    }
    void pushNull() {
        pushDataAndMask(ColType{}, 1);
    }

    size_t size() const override { return _data.size(); }
    const void* data() const override { return _data.data(); }
    void* data() override { return _data.data(); }
    const void* mask() const override { return _mask.data(); }
    void* mask() override { return _mask.data(); }

    bool isValid(size_t index) {
        if (_mask[index]) {
            return false;
        }
        return true;
    }

    ColType& operator[](size_t index) {
        return _data[index];
    }

    const ColType& operator[](size_t index) const {
        return _data[index];
    }

    void dump() {
        std::cout << _colName << std::endl;
        for (size_t i = 0; i < size(); i++) {
            if (!_mask[i]) {
                std::cout << _data[i] << std::endl;
            } else {
                std::cout << "NaN" << std::endl;
            }
        }
    }
};
}
