module;

#include <numeric>
#include <string>

export module math.rational;

import core.exceptions;

export namespace math {

class Rational {
public:
    Rational() = default;

    Rational(long long numerator)
        : numerator_(numerator),
          denominator_(1) {}

    Rational(long long numerator, long long denominator)
        : numerator_(numerator),
          denominator_(denominator) {
        normalize();
    }

    [[nodiscard]] long long numerator() const noexcept {
        return numerator_;
    }

    [[nodiscard]] long long denominator() const noexcept {
        return denominator_;
    }

    [[nodiscard]] bool isZero() const noexcept {
        return numerator_ == 0;
    }

    [[nodiscard]] std::string toString() const {
        if (denominator_ == 1) {
            return std::to_string(numerator_);
        }

        return std::to_string(numerator_) + "/" + std::to_string(denominator_);
    }

private:
    long long numerator_ = 0;
    long long denominator_ = 1;

    void normalize() {
        if (denominator_ == 0) {
            throw core::MathError("rational denominator cannot be zero");
        }

        if (numerator_ == 0) {
            denominator_ = 1;
            return;
        }

        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }

        const long long divisor = std::gcd(numerator_, denominator_);
        numerator_ /= divisor;
        denominator_ /= divisor;
    }
};

[[nodiscard]] Rational operator+(const Rational& left, const Rational& right) {
    return {
        left.numerator() * right.denominator() + right.numerator() * left.denominator(),
        left.denominator() * right.denominator()
    };
}

[[nodiscard]] Rational operator-(const Rational& left, const Rational& right) {
    return {
        left.numerator() * right.denominator() - right.numerator() * left.denominator(),
        left.denominator() * right.denominator()
    };
}

[[nodiscard]] Rational operator-(const Rational& value) {
    return {-value.numerator(), value.denominator()};
}

[[nodiscard]] Rational operator*(const Rational& left, const Rational& right) {
    return {
        left.numerator() * right.numerator(),
        left.denominator() * right.denominator()
    };
}

[[nodiscard]] Rational operator/(const Rational& left, const Rational& right) {
    if (right.isZero()) {
        throw core::MathError("division by zero");
    }

    return {
        left.numerator() * right.denominator(),
        left.denominator() * right.numerator()
    };
}

[[nodiscard]] bool operator==(const Rational& left, const Rational& right) noexcept {
    return left.numerator() == right.numerator()
        && left.denominator() == right.denominator();
}

[[nodiscard]] bool operator!=(const Rational& left, const Rational& right) noexcept {
    return !(left == right);
}

[[nodiscard]] bool operator<(const Rational& left, const Rational& right) noexcept {
    return left.numerator() * right.denominator()
        < right.numerator() * left.denominator();
}

[[nodiscard]] bool operator>(const Rational& left, const Rational& right) noexcept {
    return right < left;
}

[[nodiscard]] bool operator<=(const Rational& left, const Rational& right) noexcept {
    return !(right < left);
}

[[nodiscard]] bool operator>=(const Rational& left, const Rational& right) noexcept {
    return !(left < right);
}

}