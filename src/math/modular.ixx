module;

#include <string>

export module math.modular;

import core.exceptions;
import math.number_theory;

export namespace math {

class Modular {
public:
    Modular() = default;

    Modular(long long value, long long mod)
        : value_(modNormalize(value, mod)),
          mod_(mod) {
        if (mod <= 0) {
            throw core::MathError("modulus must be positive");
        }
    }

    [[nodiscard]] long long value() const noexcept {
        return value_;
    }

    [[nodiscard]] long long mod() const noexcept {
        return mod_;
    }

    [[nodiscard]] std::string toString() const {
        return std::to_string(value_) + " (mod " + std::to_string(mod_) + ")";
    }

    [[nodiscard]] Modular inverse() const {
        return Modular{modInverse(value_, mod_), mod_};
    }

    [[nodiscard]] Modular pow(long long exponent) const {
        return Modular{modPow(value_, exponent, mod_), mod_};
    }

    friend Modular operator+(const Modular& left, const Modular& right) {
        checkSameMod(left, right);
        return Modular{left.value_ + right.value_, left.mod_};
    }

    friend Modular operator-(const Modular& left, const Modular& right) {
        checkSameMod(left, right);
        return Modular{left.value_ - right.value_, left.mod_};
    }

    friend Modular operator-(const Modular& value) {
        return Modular{-value.value_, value.mod_};
    }

    friend Modular operator*(const Modular& left, const Modular& right) {
        checkSameMod(left, right);
        return Modular{
            modMul(left.value_, right.value_, left.mod_),
            left.mod_
        };
    }

    friend Modular operator/(const Modular& left, const Modular& right) {
        checkSameMod(left, right);
        return left * right.inverse();
    }

    friend bool operator==(const Modular& left, const Modular& right) noexcept {
        return left.value_ == right.value_
            && left.mod_ == right.mod_;
    }

    friend bool operator!=(const Modular& left, const Modular& right) noexcept {
        return !(left == right);
    }

private:
    long long value_ = 0;
    long long mod_ = 1;

    static void checkSameMod(const Modular& left, const Modular& right) {
        if (left.mod_ != right.mod_) {
            throw core::MathError("moduli do not match");
        }
    }
};

}