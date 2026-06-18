module;

#include <string>
#include <variant>

export module core.value;
import math.rational;
import math.matrix;
import math.poly;
import core.exceptions;

export namespace core {

class Value {
public:
    Value() : data_(math::Rational(0)) {}
    Value(math::Rational r) : data_(r) {}
    Value(math::Matrix m) : data_(m) {}
    Value(math::Polynomial p) : data_(p) {}
    Value(std::string s) : data_(std::move(s)) {}

    [[nodiscard]] bool isRational() const noexcept {
        return std::holds_alternative<math::Rational>(data_);
    }

    [[nodiscard]] bool isMatrix() const noexcept {
        return std::holds_alternative<math::Matrix>(data_);
    }

    [[nodiscard]] bool isPolynomial() const noexcept {
        return std::holds_alternative<math::Polynomial>(data_);
    }

    [[nodiscard]] bool isString() const noexcept {
        return std::holds_alternative<std::string>(data_);
    }

    [[nodiscard]] math::Rational asRational() const {
        return std::get<math::Rational>(data_);
    }

    [[nodiscard]] math::Matrix asMatrix() const {
        return std::get<math::Matrix>(data_);
    }

    [[nodiscard]] math::Polynomial asPolynomial() const {
        return std::get<math::Polynomial>(data_);
    }

    [[nodiscard]] std::string asString() const {
        return std::get<std::string>(data_);
    }

    [[nodiscard]] std::string toString() const {
        struct Visitor {
            std::string operator()(const math::Rational& val) const { return val.toString(); }
            std::string operator()(const math::Matrix& val) const { return val.toString(); }
            std::string operator()(const math::Polynomial& val) const { return val.toString(); }
            std::string operator()(const std::string& val) const { return val; }
        };
        return std::visit(Visitor{}, data_);
    }

private:
    std::variant<math::Rational, math::Matrix, math::Polynomial, std::string> data_;
};

[[nodiscard]] Value add(const Value& left, const Value& right) {
    if (left.isRational() && right.isRational()) {
        return Value(left.asRational() + right.asRational());
    }
    if (left.isMatrix() && right.isMatrix()) {
        return Value(left.asMatrix() + right.asMatrix());
    }
    if (left.isPolynomial() && right.isPolynomial()) {
        return Value(left.asPolynomial() + right.asPolynomial());
    }
    throw MathError("type mismatch or operation not supported for addition");
}

[[nodiscard]] Value subtract(const Value& left, const Value& right) {
    if (left.isRational() && right.isRational()) {
        return Value(left.asRational() - right.asRational());
    }
    if (left.isMatrix() && right.isMatrix()) {
        return Value(left.asMatrix() - right.asMatrix());
    }
    if (left.isPolynomial() && right.isPolynomial()) {
        return Value(left.asPolynomial() - right.asPolynomial());
    }
    throw MathError("type mismatch or operation not supported for subtraction");
}

[[nodiscard]] Value multiply(const Value& left, const Value& right) {
    if (left.isRational() && right.isRational()) {
        return Value(left.asRational() * right.asRational());
    }
    if (left.isMatrix() && right.isMatrix()) {
        return Value(left.asMatrix() * right.asMatrix());
    }
    if (left.isMatrix() && right.isRational()) {
        return Value(left.asMatrix() * right.asRational());
    }
    if (left.isRational() && right.isMatrix()) {
        return Value(left.asRational() * right.asMatrix());
    }
    if (left.isPolynomial() && right.isPolynomial()) {
        return Value(left.asPolynomial() * right.asPolynomial());
    }
    if (left.isPolynomial() && right.isRational()) {
        return Value(left.asPolynomial() * math::Polynomial{right.asRational()});
    }
    if (left.isRational() && right.isPolynomial()) {
        return Value(math::Polynomial{left.asRational()} * right.asPolynomial());
    }
    throw MathError("type mismatch or operation not supported for multiplication");
}

[[nodiscard]] Value divide(const Value& left, const Value& right) {
    if (left.isRational() && right.isRational()) {
        return Value(left.asRational() / right.asRational());
    }
    throw MathError("division is only supported for rational numbers");
}

[[nodiscard]] Value negate(const Value& val) {
    if (val.isRational()) return Value(-val.asRational());
    if (val.isMatrix()) return Value(-val.asMatrix());
    if (val.isPolynomial()) return Value(-val.asPolynomial());
    throw MathError("operation not supported");
}

}
