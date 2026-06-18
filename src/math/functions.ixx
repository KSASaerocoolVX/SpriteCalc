module;

#include <cstddef>

export module math.functions;

import math.matrix;
import math.modular;
import math.number_theory;
import math.poly;
import math.rational;

export namespace math {

    [[nodiscard]] Matrix identityMatrix(std::size_t size) {
        return Matrix::identity(size);
    }

    [[nodiscard]] Matrix transpose(const Matrix& matrix) {
        return matrix.transposed();
    }

    [[nodiscard]] Rational evaluatePolynomial(const Polynomial& polynomial, const Rational& value) {
        return polynomial.evaluate(value);
    }

    [[nodiscard]] long long greatestCommonDivisor(long long left, long long right) noexcept {
        return gcd(left, right);
    }

    [[nodiscard]] long long leastCommonMultiple(long long left, long long right) {
        return lcm(left, right);
    }

    [[nodiscard]] long long powerModulo(long long base, long long exponent, long long mod) {
        return modPow(base, exponent, mod);
    }

    [[nodiscard]] long long inverseModulo(long long value, long long mod) {
        return modInverse(value, mod);
    }

    [[nodiscard]] bool prime(long long value) noexcept {
        return isPrime(value);
    }

    [[nodiscard]] Modular modularPower(const Modular& value, long long exponent) {
        return value.pow(exponent);
    }

    [[nodiscard]] Modular modularInverse(const Modular& value) {
        return value.inverse();
    }

}