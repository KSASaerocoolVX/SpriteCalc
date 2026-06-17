// ЗАГЛУШКА

module;

#include <string>
#include <variant>

export module core.value;
import math.rational;

export namespace core {

export class Value {
public:
    Value() : data_(math::Rational(0)) {}
    Value(math::Rational r) : data_(r) {}

    [[nodiscard]] math::Rational asRational() const {
        return std::get<math::Rational>(data_);
    }

    [[nodiscard]] std::string toString() const {
        return asRational().toString(); 
    }

private:
    std::variant<math::Rational> data_; 
};

[[nodiscard]] Value add(const Value& left, const Value& right) {
    return Value(left.asRational() + right.asRational());
}
[[nodiscard]] Value subtract(const Value& left, const Value& right) {
    return Value(left.asRational() - right.asRational());
}
[[nodiscard]] Value multiply(const Value& left, const Value& right) {
    return Value(left.asRational() * right.asRational());
}
[[nodiscard]] Value divide(const Value& left, const Value& right) {
    return Value(left.asRational() / right.asRational());
}
[[nodiscard]] Value negate(const Value& val) {
    return Value(-val.asRational());
}
}