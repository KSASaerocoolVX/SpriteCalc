module;

#include <cmath>
#include <complex>
#include <cstddef>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

export module math.complex;

export namespace math {

class Complex {
public:
    Complex() = default;

    explicit Complex(double real)
        : value_(real, 0.0) {}

    Complex(double real, double imaginary)
        : value_(real, imaginary) {}

    [[nodiscard]] double real() const noexcept {
        return value_.real();
    }

    [[nodiscard]] double imaginary() const noexcept {
        return value_.imag();
    }

    [[nodiscard]] bool isReal() const noexcept {
        return std::abs(value_.imag()) <= 1e-12;
    }

    [[nodiscard]] std::string toString() const {
        std::ostringstream output;
        output << value_.real();

        if (value_.imag() >= 0.0) {
            output << "+";
        }

        output << value_.imag() << "i";
        return output.str();
    }

    friend Complex operator+(const Complex& left, const Complex& right) {
        return Complex{left.value_ + right.value_};
    }

    friend Complex operator-(const Complex& left, const Complex& right) {
        return Complex{left.value_ - right.value_};
    }

    friend Complex operator-(const Complex& value) {
        return Complex{-value.value_};
    }

    friend Complex operator*(const Complex& left, const Complex& right) {
        return Complex{left.value_ * right.value_};
    }

    friend Complex operator/(const Complex& left, const Complex& right) {
        return Complex{left.value_ / right.value_};
    }

private:
    explicit Complex(std::complex<double> value)
        : value_(value) {}

    std::complex<double> value_ = {};
};

class ComplexList {
public:
    ComplexList() = default;

    explicit ComplexList(std::vector<Complex> values)
        : values_(std::move(values)) {}

    [[nodiscard]] std::string toString() const {
        std::string result = "[";

        for (std::size_t index = 0; index < values_.size(); ++index) {
            if (index != 0) {
                result += ", ";
            }

            result += values_[index].toString();
        }

        result += "]";
        return result;
    }

private:
    std::vector<Complex> values_;
};

}
