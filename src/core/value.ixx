module;

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

export module core.value;

import core.exceptions;
import math.complex;
import math.matrix;
import math.poly;
import math.rational;

namespace core {

// Вспомогательная функция для безопасного перевода Rational в double без метода toDouble()
[[nodiscard]] double rationalToDouble(const math::Rational& r) noexcept {
    return static_cast<double>(r.numerator()) / r.denominator();
}

// Простейший перевод double в Rational для совместимости с Polynomial/Matrix
[[nodiscard]] math::Rational doubleToRational(double val) {
    double intPart;
    double fracPart = std::modf(val, &intPart);
    long long num = static_cast<long long>(intPart);
    if (fracPart == 0.0) {
        return math::Rational{num, 1};
    }
    // Округляем до 6 знаков после запятой
    long long den = 1000000;
    num = static_cast<long long>(std::round(val * den));
    return math::Rational{num, den};
}

// --- Вспомогательные функции для операций со скалярами в Polynomial ---
[[nodiscard]] math::Polynomial addScalar(const math::Polynomial& poly, const math::Rational& scalar) {
    return poly + math::Polynomial{scalar};
}
[[nodiscard]] math::Polynomial addScalar(const math::Polynomial& poly, double scalar) {
    return poly + math::Polynomial{doubleToRational(scalar)};
}
[[nodiscard]] math::Polynomial subtractScalar(const math::Polynomial& poly, const math::Rational& scalar) {
    return poly - math::Polynomial{scalar};
}
[[nodiscard]] math::Polynomial subtractScalar(const math::Polynomial& poly, double scalar) {
    return poly - math::Polynomial{doubleToRational(scalar)};
}
[[nodiscard]] math::Polynomial scalarSubtract(const math::Rational& scalar, const math::Polynomial& poly) {
    return math::Polynomial{scalar} - poly;
}
[[nodiscard]] math::Polynomial scalarSubtract(double scalar, const math::Polynomial& poly) {
    return math::Polynomial{doubleToRational(scalar)} - poly;
}
[[nodiscard]] math::Polynomial multiplyScalar(const math::Polynomial& poly, const math::Rational& scalar) {
    return poly * math::Polynomial{scalar};
}
[[nodiscard]] math::Polynomial multiplyScalar(const math::Polynomial& poly, double scalar) {
    return poly * math::Polynomial{doubleToRational(scalar)};
}
[[nodiscard]] math::Polynomial divideScalar(const math::Polynomial& poly, const math::Rational& scalar) {
    if (scalar.isZero()) {
        throw MathError("division by zero");
    }
    return poly * math::Polynomial{math::Rational{1} / scalar};
}
[[nodiscard]] math::Polynomial divideScalar(const math::Polynomial& poly, double scalar) {
    if (scalar == 0.0) {
        throw MathError("division by zero");
    }
    return divideScalar(poly, doubleToRational(scalar));
}

// --- Вспомогательные функции для операций со скалярами в Matrix ---
[[nodiscard]] math::Matrix addScalar(const math::Matrix& mat, const math::Rational& scalar) {
    math::Matrix result(mat.rows(), mat.cols());
    for (std::size_t r = 0; r < mat.rows(); ++r) {
        for (std::size_t c = 0; c < mat.cols(); ++c) {
            result.at(r, c) = mat.at(r, c) + scalar;
        }
    }
    return result;
}
[[nodiscard]] math::Matrix subtractScalar(const math::Matrix& mat, const math::Rational& scalar) {
    math::Matrix result(mat.rows(), mat.cols());
    for (std::size_t r = 0; r < mat.rows(); ++r) {
        for (std::size_t c = 0; c < mat.cols(); ++c) {
            result.at(r, c) = mat.at(r, c) - scalar;
        }
    }
    return result;
}
[[nodiscard]] math::Matrix scalarSubtract(const math::Rational& scalar, const math::Matrix& mat) {
    math::Matrix result(mat.rows(), mat.cols());
    for (std::size_t r = 0; r < mat.rows(); ++r) {
        for (std::size_t c = 0; c < mat.cols(); ++c) {
            result.at(r, c) = scalar - mat.at(r, c);
        }
    }
    return result;
}
[[nodiscard]] math::Matrix multiplyScalar(const math::Matrix& mat, const math::Rational& scalar) {
    return mat * scalar;
}
[[nodiscard]] math::Matrix divideScalar(const math::Matrix& mat, const math::Rational& scalar) {
    if (scalar.isZero()) {
        throw MathError("division by zero");
    }
    return mat * (math::Rational{1} / scalar);
}

} // namespace core

export namespace core {

class Value {
public:
    using Storage = std::variant<
        int,
        std::int64_t,
        double,
        math::Rational,
        math::Complex,
        math::ComplexList,
        math::Matrix,
        math::Polynomial
    >;

    Value()
        : data_(0) {}

    Value(int value)
        : data_(value) {}

    Value(std::int64_t value)
        : data_(value) {}

    Value(double value)
        : data_(value) {}

    Value(math::Rational value)
        : data_(std::move(value)) {}

    Value(math::Complex value)
        : data_(std::move(value)) {}

    Value(math::ComplexList value)
        : data_(std::move(value)) {}

    Value(math::Matrix value)
        : data_(std::move(value)) {}

    Value(math::Polynomial value)
        : data_(std::move(value)) {}

    [[nodiscard]] bool isInt() const noexcept {
        return std::holds_alternative<int>(data_);
    }

    [[nodiscard]] bool isDouble() const noexcept {
        return std::holds_alternative<double>(data_);
    }

    [[nodiscard]] bool isInt64() const noexcept {
        return std::holds_alternative<std::int64_t>(data_);
    }

    [[nodiscard]] bool isRational() const noexcept {
        return std::holds_alternative<math::Rational>(data_);
    }

    [[nodiscard]] bool isComplex() const noexcept {
        return std::holds_alternative<math::Complex>(data_);
    }

    [[nodiscard]] bool isComplexList() const noexcept {
        return std::holds_alternative<math::ComplexList>(data_);
    }

    [[nodiscard]] bool isMatrix() const noexcept {
        return std::holds_alternative<math::Matrix>(data_);
    }

    [[nodiscard]] bool isPolynomial() const noexcept {
        return std::holds_alternative<math::Polynomial>(data_);
    }

    [[nodiscard]] int asInt() const {
        if (!isInt()) {
            throw MathError("value is not an int");
        }
        return std::get<int>(data_);
    }

    [[nodiscard]] double asDouble() const {
        return std::visit(
            [](auto value) {
                using Type = decltype(value);

                if constexpr (std::is_same_v<Type, math::Rational>) {
                    return rationalToDouble(value);
                } else if constexpr (std::is_same_v<Type, math::Complex>) {
                    if (!value.isReal()) {
                        throw MathError("complex value cannot be converted to real scalar");
                    }
                    return value.real();
                } else if constexpr (std::is_same_v<Type, math::ComplexList>) {
                    throw MathError("complex root list cannot be converted to scalar");
                    return 0.0;
                } else if constexpr (std::is_same_v<Type, math::Matrix>) {
                    throw MathError("matrix cannot be converted to scalar");
                    return 0.0;
                } else if constexpr (std::is_same_v<Type, math::Polynomial>) {
                    throw MathError("polynomial cannot be converted to scalar");
                    return 0.0;
                } else {
                    return static_cast<double>(value);
                }
            },
            data_
        );
    }

    [[nodiscard]] std::int64_t asInt64() const {
        if (isInt64()) {
            return std::get<std::int64_t>(data_);
        }

        if (isInt()) {
            return std::get<int>(data_);
        }

        if (isRational()) {
            const auto& value = std::get<math::Rational>(data_);
            if (value.denominator() == 1) {
                return value.numerator();
            }
        }

        throw MathError("value is not an integer");
    }

    [[nodiscard]] math::Rational asRational() const {
        if (isRational()) {
            return std::get<math::Rational>(data_);
        }

        if (isInt64()) {
            return math::Rational{std::get<std::int64_t>(data_)};
        }

        if (isInt()) {
            return math::Rational{std::get<int>(data_)};
        }

        throw MathError("value is not rational-compatible");
    }

    [[nodiscard]] math::Complex asComplex() const {
        if (isComplex()) {
            return std::get<math::Complex>(data_);
        }

        if (isDouble()) {
            return math::Complex{std::get<double>(data_)};
        }

        if (isRational()) {
            return math::Complex{rationalToDouble(std::get<math::Rational>(data_))};
        }

        if (isInt64()) {
            return math::Complex{static_cast<double>(std::get<std::int64_t>(data_))};
        }

        if (isInt()) {
            return math::Complex{static_cast<double>(std::get<int>(data_))};
        }

        throw MathError("value is not complex-compatible");
    }

    [[nodiscard]] const math::Matrix& asMatrix() const {
        if (!isMatrix()) {
            throw MathError("value is not a matrix");
        }

        return std::get<math::Matrix>(data_);
    }

    [[nodiscard]] const math::Polynomial& asPolynomial() const {
        if (!isPolynomial()) {
            throw MathError("value is not a polynomial");
        }

        return std::get<math::Polynomial>(data_);
    }

    [[nodiscard]] std::string toString() const {
        if (isInt()) {
            return std::to_string(std::get<int>(data_));
        }

        if (isInt64()) {
            return std::to_string(std::get<std::int64_t>(data_));
        }

        if (isRational()) {
            return std::get<math::Rational>(data_).toString();
        }

        if (isComplex()) {
            return std::get<math::Complex>(data_).toString();
        }

        if (isComplexList()) {
            return std::get<math::ComplexList>(data_).toString();
        }

        if (isMatrix()) {
            return std::get<math::Matrix>(data_).toString();
        }

        if (isPolynomial()) {
            return std::get<math::Polynomial>(data_).toString();
        }

        auto value = std::get<double>(data_);
        if (value == 0.0) {
            value = 0.0;
        }

        std::ostringstream output;
        output << std::setprecision(15) << value;
        auto text = output.str();

        if (text.find('.') != std::string::npos) {
            while (!text.empty() && text.back() == '0') {
                text.pop_back();
            }
            if (!text.empty() && text.back() == '.') {
                text.pop_back();
            }
        }

        return text.empty() ? "0" : text;
    }

    [[nodiscard]] const Storage& storage() const noexcept {
        return data_;
    }

private:
    Storage data_;
};

[[nodiscard]] Value makeIntegerValue(std::int64_t value) {
    if (
        value >= static_cast<std::int64_t>(std::numeric_limits<int>::min()) &&
        value <= static_cast<std::int64_t>(std::numeric_limits<int>::max())
    ) {
        return Value{static_cast<int>(value)};
    }

    return Value{value};
}

[[nodiscard]] Value parseIntegerLiteral(const std::string& text) {
    try {
        std::size_t parsed = 0;
        const auto value = std::stoll(text, &parsed);
        if (parsed == text.size()) {
            return makeIntegerValue(value);
        }
    } catch (const std::out_of_range&) {
        throw SyntaxError("integer literal is out of range: " + text);
    } catch (const std::invalid_argument&) {
        throw SyntaxError("invalid integer literal: " + text);
    }

    throw SyntaxError("invalid integer literal: " + text);
}

[[nodiscard]] Value negate(const Value& value) {
    return std::visit(
        [](auto item) -> Value {
            using Type = decltype(item);

            if constexpr (std::is_same_v<Type, int>) {
                if (item == std::numeric_limits<int>::min()) {
                    return Value{-static_cast<std::int64_t>(item)};
                }
                return Value{-item};
            } else if constexpr (std::is_same_v<Type, std::int64_t>) {
                if (item == std::numeric_limits<std::int64_t>::min()) {
                    throw MathError("integer overflow");
                }
                return Value{-item};
            } else if constexpr (std::is_same_v<Type, math::Rational>) {
                return Value{-item};
            } else if constexpr (std::is_same_v<Type, math::Complex>) {
                return Value{-item};
            } else if constexpr (std::is_same_v<Type, math::ComplexList>) {
                throw MathError("complex root list cannot be negated");
            } else if constexpr (std::is_same_v<Type, math::Matrix>) {
                return Value{-item};
            } else if constexpr (std::is_same_v<Type, math::Polynomial>) {
                return Value{-item};
            } else {
                return Value{-item};
            }
        },
        value.storage()
    );
}

[[nodiscard]] Value add(const Value& left, const Value& right) {
    return std::visit(
        [](auto lhs, auto rhs) -> Value {
            using Left = decltype(lhs);
            using Right = decltype(rhs);

            if constexpr (std::is_integral_v<Left> && std::is_integral_v<Right>) {
                return makeIntegerValue(static_cast<std::int64_t>(lhs) + static_cast<std::int64_t>(rhs));
            } else if constexpr (std::is_same_v<Left, math::Matrix> && std::is_same_v<Right, math::Matrix>) {
                return Value{lhs + rhs};
            } else if constexpr (std::is_same_v<Left, math::Polynomial> && std::is_same_v<Right, math::Polynomial>) {
                return Value{lhs + rhs};
            } else if constexpr (std::is_same_v<Left, math::Polynomial> && std::is_same_v<Right, double>) {
                return Value{addScalar(lhs, rhs)};
            } else if constexpr (std::is_same_v<Left, math::Polynomial>) {
                return Value{addScalar(lhs, Value{rhs}.asRational())};
            } else if constexpr (std::is_same_v<Right, math::Polynomial> && std::is_same_v<Left, double>) {
                return Value{addScalar(rhs, lhs)};
            } else if constexpr (std::is_same_v<Right, math::Polynomial>) {
                return Value{addScalar(rhs, Value{lhs}.asRational())};
            } else if constexpr (std::is_same_v<Left, math::Matrix>) {
                return Value{addScalar(lhs, Value{rhs}.asRational())};
            } else if constexpr (std::is_same_v<Right, math::Matrix>) {
                return Value{addScalar(rhs, Value{lhs}.asRational())};
            } else if constexpr (std::is_same_v<Left, math::Complex> || std::is_same_v<Right, math::Complex>) {
                return Value{Value{lhs}.asComplex() + Value{rhs}.asComplex()};
            } else if constexpr (std::is_same_v<Left, double> || std::is_same_v<Right, double>) {
                return Value{Value{lhs}.asDouble() + Value{rhs}.asDouble()};
            } else if constexpr (std::is_same_v<Left, math::Rational> || std::is_same_v<Right, math::Rational>) {
                return Value{Value{lhs}.asRational() + Value{rhs}.asRational()};
            } else {
                return makeIntegerValue(Value{lhs}.asInt64() + Value{rhs}.asInt64());
            }
        },
        left.storage(),
        right.storage()
    );
}

[[nodiscard]] Value subtract(const Value& left, const Value& right) {
    return std::visit(
        [](auto lhs, auto rhs) -> Value {
            using Left = decltype(lhs);
            using Right = decltype(rhs);

            if constexpr (std::is_integral_v<Left> && std::is_integral_v<Right>) {
                return makeIntegerValue(static_cast<std::int64_t>(lhs) - static_cast<std::int64_t>(rhs));
            } else if constexpr (std::is_same_v<Left, math::Matrix> && std::is_same_v<Right, math::Matrix>) {
                return Value{lhs - rhs};
            } else if constexpr (std::is_same_v<Left, math::Polynomial> && std::is_same_v<Right, math::Polynomial>) {
                return Value{lhs - rhs};
            } else if constexpr (std::is_same_v<Left, math::Polynomial> && std::is_same_v<Right, double>) {
                return Value{subtractScalar(lhs, rhs)};
            } else if constexpr (std::is_same_v<Left, math::Polynomial>) {
                return Value{subtractScalar(lhs, Value{rhs}.asRational())};
            } else if constexpr (std::is_same_v<Right, math::Polynomial> && std::is_same_v<Left, double>) {
                return Value{scalarSubtract(lhs, rhs)};
            } else if constexpr (std::is_same_v<Right, math::Polynomial>) {
                return Value{scalarSubtract(Value{lhs}.asRational(), rhs)};
            } else if constexpr (std::is_same_v<Left, math::Matrix>) {
                return Value{subtractScalar(lhs, Value{rhs}.asRational())};
            } else if constexpr (std::is_same_v<Right, math::Matrix>) {
                return Value{scalarSubtract(Value{lhs}.asRational(), rhs)};
            } else if constexpr (std::is_same_v<Left, math::Complex> || std::is_same_v<Right, math::Complex>) {
                return Value{Value{lhs}.asComplex() - Value{rhs}.asComplex()};
            } else if constexpr (std::is_same_v<Left, double> || std::is_same_v<Right, double>) {
                return Value{Value{lhs}.asDouble() - Value{rhs}.asDouble()};
            } else if constexpr (std::is_same_v<Left, math::Rational> || std::is_same_v<Right, math::Rational>) {
                return Value{Value{lhs}.asRational() - Value{rhs}.asRational()};
            } else {
                return makeIntegerValue(Value{lhs}.asInt64() - Value{rhs}.asInt64());
            }
        },
        left.storage(),
        right.storage()
    );
}

[[nodiscard]] Value multiply(const Value& left, const Value& right) {
    return std::visit(
        [](auto lhs, auto rhs) -> Value {
            using Left = decltype(lhs);
            using Right = decltype(rhs);

            if constexpr (std::is_integral_v<Left> && std::is_integral_v<Right>) {
                return makeIntegerValue(static_cast<std::int64_t>(lhs) * static_cast<std::int64_t>(rhs));
            } else if constexpr (std::is_same_v<Left, math::Matrix> && std::is_same_v<Right, math::Matrix>) {
                return Value{lhs * rhs};
            } else if constexpr (std::is_same_v<Left, math::Polynomial> && std::is_same_v<Right, math::Polynomial>) {
                return Value{lhs * rhs};
            } else if constexpr (std::is_same_v<Left, math::Polynomial> && std::is_same_v<Right, double>) {
                return Value{multiplyScalar(lhs, rhs)};
            } else if constexpr (std::is_same_v<Left, math::Polynomial>) {
                return Value{multiplyScalar(lhs, Value{rhs}.asRational())};
            } else if constexpr (std::is_same_v<Right, math::Polynomial> && std::is_same_v<Left, double>) {
                return Value{multiplyScalar(rhs, lhs)};
            } else if constexpr (std::is_same_v<Right, math::Polynomial>) {
                return Value{multiplyScalar(rhs, Value{lhs}.asRational())};
            } else if constexpr (std::is_same_v<Left, math::Matrix>) {
                return Value{multiplyScalar(lhs, Value{rhs}.asRational())};
            } else if constexpr (std::is_same_v<Right, math::Matrix>) {
                return Value{multiplyScalar(rhs, Value{lhs}.asRational())};
            } else if constexpr (std::is_same_v<Left, math::Complex> || std::is_same_v<Right, math::Complex>) {
                return Value{Value{lhs}.asComplex() * Value{rhs}.asComplex()};
            } else if constexpr (std::is_same_v<Left, double> || std::is_same_v<Right, double>) {
                return Value{Value{lhs}.asDouble() * Value{rhs}.asDouble()};
            } else if constexpr (std::is_same_v<Left, math::Rational> || std::is_same_v<Right, math::Rational>) {
                return Value{Value{lhs}.asRational() * Value{rhs}.asRational()};
            } else {
                return makeIntegerValue(Value{lhs}.asInt64() * Value{rhs}.asInt64());
            }
        },
        left.storage(),
        right.storage()
    );
}

[[nodiscard]] Value divide(const Value& left, const Value& right) {
    if (left.isMatrix() && right.isMatrix()) {
        throw MathError("matrix division is not supported");
    }

    if (left.isPolynomial() && right.isPolynomial()) {
        throw MathError("polynomial division is not supported");
    }

    if (left.isPolynomial()) {
        if (right.isDouble()) {
            return Value{divideScalar(left.asPolynomial(), right.asDouble())};
        }

        return Value{divideScalar(left.asPolynomial(), right.asRational())};
    }

    if (right.isPolynomial()) {
        throw MathError("scalar divided by polynomial is not supported");
    }

    if (left.isMatrix()) {
        return Value{divideScalar(left.asMatrix(), right.asRational())};
    }

    if (right.isMatrix()) {
        throw MathError("scalar divided by matrix is not supported");
    }

    if (left.isComplex() || right.isComplex()) {
        return Value{left.asComplex() / right.asComplex()};
    }

    if (!left.isDouble() && !right.isDouble()) {
        return Value{left.asRational() / right.asRational()};
    }

    const auto divisor = right.asDouble();
    if (std::abs(divisor) <= std::numeric_limits<double>::epsilon()) {
        throw MathError("division by zero");
    }
    return Value{left.asDouble() / divisor};
}

} // namespace core
