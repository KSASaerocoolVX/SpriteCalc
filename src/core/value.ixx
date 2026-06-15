// ЗАГЛУШКА

module;

#include <string>

export module core.value;

export namespace core {

class Value {
public:
    Value() = default;
};

//todo это дополнить
[[nodiscard]] Value add(const Value& left, const Value& right) { return left; }
[[nodiscard]] Value subtract(const Value& left, const Value& right) { return left; }
[[nodiscard]] Value multiply(const Value& left, const Value& right) { return left; }
[[nodiscard]] Value divide(const Value& left, const Value& right) { return left; }
[[nodiscard]] Value negate(const Value& val) { return val; }
}