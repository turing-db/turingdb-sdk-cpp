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

    bool _boolean;
};

enum ColumnType : uint8_t {
    INT,
    UINT,
    DOUBLE,
    STRING,
    BOOL,
    FLOAT
    // Add more types as needed
};

template <typename T>
concept SupportedColumnType = std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t> || std::is_same_v<T, double> || std::is_same_v<T, std::string> || std::is_same_v<T, CustomBool> || std::is_same_v<T, float>;


class TypedColumn {
public:
    virtual ~TypedColumn() = default;
    virtual std::size_t size() const = 0;
    virtual void* data() = 0;
    virtual const void* data() const = 0;

    virtual ColumnType column_type() const = 0;
};

template <SupportedColumnType ColType>
class Column : public TypedColumn {
private:
    std::vector<std::optional<ColType>> _data;
    std::string _colName;

    // Map C++ types to enum values
    static constexpr ColumnType get_column_type() {
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
        } else if constexpr (std::is_same_v<ColType, float>) {
            return ColumnType::FLOAT;
        }
    }

public:
    Column() = default;

    explicit Column(std::string colName)
    :_colName(std::move(colName))
    {
    }
    // Maybe do an move constructor
    explicit Column(std::vector<ColType> col) {
        _data = col;
    }

    explicit Column(const std::vector<bool>& col)
        requires std::same_as<ColType, CustomBool>
    {
        _data.reserve(col.size());
        for (bool b : col) {
            _data.emplace_back(b);
        }
    }


    ColumnType column_type() const override {
        return get_column_type();
    }

    std::vector<std::optional<ColType>>& get() { return _data; }
    const std::vector<std::optional<ColType>>& get() const { return _data; }
    void push_back(const ColType& value) { _data.push_back(value); }
    void push_back(std::nullopt_t value) { _data.push_back(value); }

    size_t size() const override { return _data.size(); }
    void* data() override { return _data.data(); }
    const void* data() const override { return _data.data(); }

    std::optional<ColType>& operator[](size_t index) {
        return _data[index];
    }

    const std::optional<ColType>& operator[](size_t index) const {
        return _data[index];
    }

    template <typename... Args>
    auto insert(Args&&... args) -> decltype(_data.insert(std::forward<Args>(args)...)) {
        return _data.insert(std::forward<Args>(args)...);
    }

    auto begin() -> decltype(_data.begin()) {
        return _data.begin();
    }

    auto end() -> decltype(_data.end()) {
        return _data.end();
    }

    void dump() {
        std::cout << _colName << std::endl;
        for (size_t i = 0; i < size(); i++) {
            if (_data[i].has_value()) {
                std::cout << _data[i].value() << std::endl;
            } else {
                std::cout << "NaN" << std::endl;
            }
        }
    }
};
}
