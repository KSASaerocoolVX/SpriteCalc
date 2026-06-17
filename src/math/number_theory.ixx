module;

#include <cstdlib>

export module math.number_theory;

import core.exceptions;

export namespace math {

[[nodiscard]] long long absValue(long long value) noexcept {
    return value < 0 ? -value : value;
}

[[nodiscard]] long long gcd(long long left, long long right) noexcept {
    left = absValue(left);
    right = absValue(right);

    while (right != 0) {
        const long long next = left % right;
        left = right;
        right = next;
    }

    return left;
}

[[nodiscard]] long long lcm(long long left, long long right) {
    if (left == 0 || right == 0) {
        return 0;
    }

    return absValue(left / gcd(left, right) * right);
}

struct ExtendedGcdResult {
    long long gcd = 0;
    long long x = 0;
    long long y = 0;
};

[[nodiscard]] ExtendedGcdResult extendedGcd(long long left, long long right) noexcept {
    long long oldR = left;
    long long r = right;

    long long oldX = 1;
    long long x = 0;

    long long oldY = 0;
    long long y = 1;

    while (r != 0) {
        const long long quotient = oldR / r;

        const long long nextR = oldR - quotient * r;
        oldR = r;
        r = nextR;

        const long long nextX = oldX - quotient * x;
        oldX = x;
        x = nextX;

        const long long nextY = oldY - quotient * y;
        oldY = y;
        y = nextY;
    }

    if (oldR < 0) {
        oldR = -oldR;
        oldX = -oldX;
        oldY = -oldY;
    }

    return {oldR, oldX, oldY};
}

[[nodiscard]] bool areCoprime(long long left, long long right) noexcept {
    return gcd(left, right) == 1;
}

[[nodiscard]] long long modNormalize(long long value, long long mod) {
    if (mod <= 0) {
        throw core::MathError("modulus must be positive");
    }

    value %= mod;

    if (value < 0) {
        value += mod;
    }

    return value;
}

[[nodiscard]] long long modPow(long long base, long long exponent, long long mod) {
    if (mod <= 0) {
        throw core::MathError("modulus must be positive");
    }

    if (exponent < 0) {
        throw core::MathError("negative exponent is not supported");
    }

    long long result = 1 % mod;
    base = modNormalize(base, mod);

    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = static_cast<long long>((__int128)result * base % mod);
        }

        base = static_cast<long long>((__int128)base * base % mod);
        exponent /= 2;
    }

    return result;
}

[[nodiscard]] long long modInverse(long long value, long long mod) {
    if (mod <= 0) {
        throw core::MathError("modulus must be positive");
    }

    const ExtendedGcdResult result = extendedGcd(value, mod);

    if (result.gcd != 1) {
        throw core::MathError("modular inverse does not exist");
    }

    return modNormalize(result.x, mod);
}

[[nodiscard]] bool isPrime(long long value) noexcept {
    if (value < 2) {
        return false;
    }

    if (value == 2) {
        return true;
    }

    if (value % 2 == 0) {
        return false;
    }

    for (long long divisor = 3; divisor <= value / divisor; divisor += 2) {
        if (value % divisor == 0) {
            return false;
        }
    }

    return true;
}

}