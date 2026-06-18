module;

#include <cstddef>
#include <string>
#include <vector>

export module math.poly;

import math.rational;

export namespace math {

class Polynomial {
public:
    Polynomial() = default;

    Polynomial(Rational constant)
        : coefficients_{constant} {
        normalize();
    }

    Polynomial(std::vector<Rational> coefficients)
        : coefficients_(std::move(coefficients)) {
        normalize();
    }

    [[nodiscard]] bool isZero() const noexcept {
        return coefficients_.empty();
    }

    [[nodiscard]] std::size_t degree() const {
        if (isZero()) {
            return 0;
        }

        return coefficients_.size() - 1;
    }

    [[nodiscard]] Rational coefficient(std::size_t power) const {
        if (power >= coefficients_.size()) {
            return Rational{};
        }

        return coefficients_[power];
    }

    [[nodiscard]] Rational evaluate(const Rational& x) const {
        Rational result;

        for (std::size_t i = coefficients_.size(); i > 0; --i) {
            result = result * x + coefficients_[i - 1];
        }

        return result;
    }

    [[nodiscard]] std::string toString() const {
        if (isZero()) {
            return "0";
        }

        std::string result;

<<<<<<< HEAD
        for (std::size_t index = 0; index < coefficients_.size(); ++index) {
=======
        for (std::size_t power = coefficients_.size(); power > 0; --power) {
            const std::size_t index = power - 1;
>>>>>>> main
            const Rational current = coefficients_[index];

            if (current.isZero()) {
                continue;
            }

            if (!result.empty()) {
                result += " + ";
            }

            result += current.toString();

            if (index >= 1) {
                result += "*x";
            }

            if (index >= 2) {
                result += "^" + std::to_string(index);
            }
        }

        return result;
    }

    friend Polynomial operator+(const Polynomial& left, const Polynomial& right) {
        const std::size_t size = maxSize(left.coefficients_.size(), right.coefficients_.size());
        std::vector<Rational> result(size);

        for (std::size_t i = 0; i < size; ++i) {
            result[i] = left.coefficient(i) + right.coefficient(i);
        }

        return Polynomial{std::move(result)};
    }

    friend Polynomial operator-(const Polynomial& left, const Polynomial& right) {
        const std::size_t size = maxSize(left.coefficients_.size(), right.coefficients_.size());
        std::vector<Rational> result(size);

        for (std::size_t i = 0; i < size; ++i) {
            result[i] = left.coefficient(i) - right.coefficient(i);
        }

        return Polynomial{std::move(result)};
    }

    friend Polynomial operator-(const Polynomial& polynomial) {
        std::vector<Rational> result(polynomial.coefficients_.size());

        for (std::size_t i = 0; i < polynomial.coefficients_.size(); ++i) {
            result[i] = -polynomial.coefficients_[i];
        }

        return Polynomial{std::move(result)};
    }

    friend Polynomial operator*(const Polynomial& left, const Polynomial& right) {
        if (left.isZero() || right.isZero()) {
            return Polynomial{};
        }

        std::vector<Rational> result(left.degree() + right.degree() + 1);

        for (std::size_t i = 0; i < left.coefficients_.size(); ++i) {
            for (std::size_t j = 0; j < right.coefficients_.size(); ++j) {
                result[i + j] = result[i + j] + left.coefficients_[i] * right.coefficients_[j];
            }
        }

        return Polynomial{std::move(result)};
    }

    friend bool operator==(const Polynomial& left, const Polynomial& right) noexcept {
        return left.coefficients_ == right.coefficients_;
    }

    friend bool operator!=(const Polynomial& left, const Polynomial& right) noexcept {
        return !(left == right);
    }

<<<<<<< HEAD
    [[nodiscard]] Polynomial derivative() const {
        if (coefficients_.size() <= 1) {
            return Polynomial{};
        }
        std::vector<Rational> result(coefficients_.size() - 1);
        for (std::size_t i = 0; i < result.size(); ++i) {
            result[i] = coefficients_[i + 1] * Rational{static_cast<long long>(i + 1)};
        }
        return Polynomial{std::move(result)};
    }

    [[nodiscard]] Polynomial integral() const {
        std::vector<Rational> result(coefficients_.size() + 1);
        result[0] = Rational{0};
        for (std::size_t i = 1; i < result.size(); ++i) {
            result[i] = coefficients_[i - 1] / Rational{static_cast<long long>(i)};
        }
        return Polynomial{std::move(result)};
    }

=======
>>>>>>> main
private:
    std::vector<Rational> coefficients_;

    void normalize() {
        while (!coefficients_.empty() && coefficients_.back().isZero()) {
            coefficients_.pop_back();
        }
    }

    static std::size_t maxSize(std::size_t left, std::size_t right) noexcept {
        return left > right ? left : right;
    }
};

}