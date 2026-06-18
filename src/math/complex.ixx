module;

#include <string>

export module math.complex;

import core.exceptions;
import math.rational;

export namespace math {

class Complex {
public:
    Complex() = default;

    Complex(Rational real)
        : real_(real),
          imaginary_(0) {}

    Complex(Rational real, Rational imaginary)
        : real_(real),
          imaginary_(imaginary) {}

    [[nodiscard]] const Rational& real() const noexcept {
        return real_;
    }

    [[nodiscard]] const Rational& imaginary() const noexcept {
        return imaginary_;
    }

    [[nodiscard]] bool isReal() const noexcept {
        return imaginary_.isZero();
    }

    [[nodiscard]] bool isZero() const noexcept {
        return real_.isZero() && imaginary_.isZero();
    }

    [[nodiscard]] Complex conjugated() const {
        return Complex{real_, -imaginary_};
    }

    [[nodiscard]] Rational normSquared() const {
        return real_ * real_ + imaginary_ * imaginary_;
    }

    [[nodiscard]] std::string toString() const {
        if (imaginary_.isZero()) {
            return real_.toString();
        }

        if (real_.isZero()) {
<<<<<<< HEAD
            if (imaginary_ == Rational(1)) return "i";
            if (imaginary_ == Rational(-1)) return "-i";
            return imaginary_.toString() + "i";
        }

        if (imaginary_ == Rational(1)) {
            return real_.toString() + " + i";
        }
        if (imaginary_ == Rational(-1)) {
            return real_.toString() + " - i";
        }

=======
            return imaginary_.toString() + "i";
        }

>>>>>>> main
        if (imaginary_ < Rational{0}) {
            return real_.toString() + " - " + (-imaginary_).toString() + "i";
        }

        return real_.toString() + " + " + imaginary_.toString() + "i";
    }

    friend Complex operator+(const Complex& left, const Complex& right) {
        return Complex{
            left.real_ + right.real_,
            left.imaginary_ + right.imaginary_
        };
    }

    friend Complex operator-(const Complex& left, const Complex& right) {
        return Complex{
            left.real_ - right.real_,
            left.imaginary_ - right.imaginary_
        };
    }

    friend Complex operator-(const Complex& value) {
        return Complex{-value.real_, -value.imaginary_};
    }

    friend Complex operator*(const Complex& left, const Complex& right) {
        return Complex{
            left.real_ * right.real_ - left.imaginary_ * right.imaginary_,
            left.real_ * right.imaginary_ + left.imaginary_ * right.real_
        };
    }

    friend Complex operator/(const Complex& left, const Complex& right) {
        if (right.isZero()) {
            throw core::MathError("division by zero");
        }

        const Rational denominator = right.normSquared();

        return Complex{
            (left.real_ * right.real_ + left.imaginary_ * right.imaginary_) / denominator,
            (left.imaginary_ * right.real_ - left.real_ * right.imaginary_) / denominator
        };
    }

    friend bool operator==(const Complex& left, const Complex& right) noexcept {
        return left.real_ == right.real_
            && left.imaginary_ == right.imaginary_;
    }

    friend bool operator!=(const Complex& left, const Complex& right) noexcept {
        return !(left == right);
    }

private:
    Rational real_;
    Rational imaginary_;
};

}