module;

#include <string>
#include <variant>

export module core.value;
import math.rational;
import math.complex;
import math.matrix;
import math.poly;
import core.exceptions;

export namespace core {

class Value {
public:
    Value() : data_(math::Rational(0)) {}
    Value(math::Rational r) : data_(r) {}
    Value(math::Complex c) : data_(c) {}
    Value(math::Matrix m) : data_(m) {}
    Value(math::Polynomial p) : data_(p) {}

    [[nodiscard]] bool isRational() const noexcept {
        return std::holds_alternative<math::Rational>(data_);
    }

    [[nodiscard]] bool isComplex() const noexcept {
        return std::holds_alternative<math::Complex>(data_);
    }

    [[nodiscard]] bool isMatrix() const noexcept {
        return std::holds_alternative<math::Matrix>(data_);
    }

    [[nodiscard]] bool isPolynomial() const noexcept {
        return std::holds_alternative<math::Polynomial>(data_);
    }

    [[nodiscard]] math::Rational asRational() const {
        return std::get<math::Rational>(data_);
    }

    [[nodiscard]] math::Complex asComplex() const {
        return std::get<math::Complex>(data_);
    }

    [[nodiscard]] math::Matrix asMatrix() const {
        return std::get<math::Matrix>(data_);
    }

    [[nodiscard]] math::Polynomial asPolynomial() const {
        return std::get<math::Polynomial>(data_);
    }

    [[nodiscard]] std::string toString() const {
        return std::visit([](const auto& val) { return val.toString(); }, data_);
    }

private:
    std::variant<math::Rational, math::Complex, math::Matrix, math::Polynomial> data_;
};

[[nodiscard]] Value add(const Value& left, const Value& right) {
    if (left.isRational() && right.isRational()) {
        return Value(left.asRational() + right.asRational());
    }
    if (left.isComplex() && right.isComplex()) {
        return Value(left.asComplex() + right.asComplex());
    }
    if (left.isComplex() && right.isRational()) {
        return Value(left.asComplex() + math::Complex(right.asRational()));
    }
    if (left.isRational() && right.isComplex()) {
        return Value(math::Complex(left.asRational()) + right.asComplex());
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
    if (left.isComplex() && right.isComplex()) {
        return Value(left.asComplex() - right.asComplex());
    }
    if (left.isComplex() && right.isRational()) {
        return Value(left.asComplex() - math::Complex(right.asRational()));
    }
    if (left.isRational() && right.isComplex()) {
        return Value(math::Complex(left.asRational()) - right.asComplex());
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
    if (left.isComplex() && right.isComplex()) {
        return Value(left.asComplex() * right.asComplex());
    }
    if (left.isComplex() && right.isRational()) {
        return Value(left.asComplex() * math::Complex(right.asRational()));
    }
    if (left.isRational() && right.isComplex()) {
        return Value(math::Complex(left.asRational()) * right.asComplex());
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
    if (left.isComplex() && right.isComplex()) {
        return Value(left.asComplex() / right.asComplex());
    }
    if (left.isComplex() && right.isRational()) {
        return Value(left.asComplex() / math::Complex(right.asRational()));
    }
    if (left.isRational() && right.isComplex()) {
        return Value(math::Complex(left.asRational()) / right.asComplex());
    }
    throw MathError("division is only supported for rational or complex numbers");
}

[[nodiscard]] Value negate(const Value& val) {
    if (val.isRational()) return Value(-val.asRational());
    if (val.isComplex()) return Value(-val.asComplex());
    if (val.isMatrix()) return Value(-val.asMatrix());
    if (val.isPolynomial()) return Value(-val.asPolynomial());
    throw MathError("operation not supported");
}

}
