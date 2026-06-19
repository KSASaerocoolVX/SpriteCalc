module;

#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>

export module math.dispatcher;

import core.exceptions;
import core.value;
import math.rational;
import math.matrix;
import math.poly;
import math.number_theory;
import math.complex;
import math.functions;

export namespace math {

// Helper to convert double to math::Rational using continued fractions
inline math::Rational doubleToRational(double val) {
    if (std::isnan(val) || std::isinf(val)) {
        throw core::MathError("result is not a real number");
    }
    const double eps = 1e-9;
    double x = val;
    long long a = static_cast<long long>(std::floor(x));
    long long h0 = 1, h1 = a;
    long long k0 = 0, k1 = 1;
    while (std::abs(x - a) > eps && k1 < 1000000) {
        if (std::abs(x - a) < 1e-12) break;
        x = 1.0 / (x - a);
        a = static_cast<long long>(std::floor(x));
        long long h2 = a * h1 + h0;
        long long k2 = a * k1 + k0;
        if (std::abs(h2) > 1000000000LL || std::abs(k2) > 1000000000LL) {
            break;
        }
        h0 = h1; h1 = h2;
        k0 = k1; k1 = k2;
    }
    return math::Rational(h1, k1);
}

inline core::Value evaluateFunction(const std::string& name, const std::vector<core::Value>& args, std::size_t argumentCount) {

                    if (name == "phi") {
                        if (argumentCount != 1) throw core::SyntaxError("phi expects 1 argument");
                        
                        const auto arg = args[0];
                        if (!arg.isRational()) throw core::MathError("phi expects an integer argument");
                        long long val = arg.asRational().numerator();
                        if (arg.asRational().denominator() != 1) throw core::MathError("phi expects an integer argument");
                        return core::Value(math::Rational(math::phi(val)));
                    }
                    else if (name == "det") {
                        if (argumentCount != 1) throw core::SyntaxError("det expects 1 argument");
                        
                        const auto arg = args[0];
                        if (!arg.isMatrix()) throw core::MathError("det expects a matrix argument");
                        return core::Value(arg.asMatrix().determinant());
                    }
                    else if (name == "derivative") {
                        if (argumentCount != 1) throw core::SyntaxError("derivative expects 1 argument");
                        
                        const auto arg = args[0];
                        if (!arg.isPolynomial()) throw core::MathError("derivative expects a polynomial argument");
                        return core::Value(arg.asPolynomial().derivative());
                    }
                    else if (name == "integral") {
                        if (argumentCount != 1) throw core::SyntaxError("integral expects 1 argument");
                        
                        const auto arg = args[0];
                        if (!arg.isPolynomial()) throw core::MathError("integral expects a polynomial argument");
                        return core::Value(arg.asPolynomial().integral());
                    }
                    else if (name == "gcd") {
                        if (argumentCount != 2) throw core::SyntaxError("gcd expects 2 arguments");
                        
                        const auto left = args[0];
                        const auto right = args[1];
                        if (!left.isRational() || !right.isRational()) throw core::MathError("gcd expects integer arguments");
                        if (left.asRational().denominator() != 1 || right.asRational().denominator() != 1) throw core::MathError("gcd expects integer arguments");
                        return core::Value(math::Rational(math::gcd(left.asRational().numerator(), right.asRational().numerator())));
                    }
                    else if (name == "lcm") {
                        if (argumentCount != 2) throw core::SyntaxError("lcm expects 2 arguments");
                        
                        const auto left = args[0];
                        const auto right = args[1];
                        if (!left.isRational() || !right.isRational()) throw core::MathError("lcm expects integer arguments");
                        if (left.asRational().denominator() != 1 || right.asRational().denominator() != 1) throw core::MathError("lcm expects integer arguments");
                        return core::Value(math::Rational(math::lcm(left.asRational().numerator(), right.asRational().numerator())));
                    }
                    else if (name == "sqrt") {
                        if (argumentCount != 1) throw core::SyntaxError("sqrt expects 1 argument");
                        
                        const auto arg = args[0];
                        if (arg.isRational()) {
                            auto r = arg.asRational();
                            long long n = r.numerator();
                            long long d = r.denominator();
                            if (n >= 0) {
                                long long sqrt_n = static_cast<long long>(std::round(std::sqrt(n)));
                                long long sqrt_d = static_cast<long long>(std::round(std::sqrt(d)));
                                if (sqrt_n * sqrt_n == n && sqrt_d * sqrt_d == d) {
                                    return core::Value(math::Rational(sqrt_n, sqrt_d));
                                } else {
                                    throw core::MathError("exact square root is not rational");
                                }
                            } else {
                                long long pos_n = -n;
                                long long sqrt_n = static_cast<long long>(std::round(std::sqrt(pos_n)));
                                long long sqrt_d = static_cast<long long>(std::round(std::sqrt(d)));
                                if (sqrt_n * sqrt_n == pos_n && sqrt_d * sqrt_d == d) {
                                    return core::Value(math::Complex(math::Rational(0), math::Rational(sqrt_n, sqrt_d)));
                                } else {
                                    throw core::MathError("exact square root is not a simple complex number");
                                }
                            }
                        } else if (arg.isComplex()) {
                            auto c = arg.asComplex();
                            if (c.isReal()) {
                                auto r = c.real();
                                long long n = r.numerator();
                                long long d = r.denominator();
                                if (n >= 0) {
                                    long long sqrt_n = static_cast<long long>(std::round(std::sqrt(n)));
                                    long long sqrt_d = static_cast<long long>(std::round(std::sqrt(d)));
                                    if (sqrt_n * sqrt_n == n && sqrt_d * sqrt_d == d) {
                                        return core::Value(math::Rational(sqrt_n, sqrt_d));
                                    } else {
                                        throw core::MathError("exact square root is not rational");
                                    }
                                } else {
                                    long long pos_n = -n;
                                    long long sqrt_n = static_cast<long long>(std::round(std::sqrt(pos_n)));
                                    long long sqrt_d = static_cast<long long>(std::round(std::sqrt(d)));
                                    if (sqrt_n * sqrt_n == pos_n && sqrt_d * sqrt_d == d) {
                                        return core::Value(math::Complex(math::Rational(0), math::Rational(sqrt_n, sqrt_d)));
                                    } else {
                                        throw core::MathError("exact square root is not a simple complex number");
                                    }
                                }
                            } else {
                                throw core::MathError("sqrt of arbitrary complex numbers is not supported");
                            }
                        } else {
                            throw core::MathError("sqrt expects a rational or real complex number");
                        }
                    }
                    else if (name == "pow") {
                        if (argumentCount != 2) throw core::SyntaxError("pow expects 2 arguments");
                        
                        const auto left = args[0];
                        const auto right = args[1];
                        return core::power(left, right);
                    }
                    else if (name == "sin") {
                        if (argumentCount != 1) throw core::SyntaxError("sin expects 1 argument");
                        
                        const auto arg = args[0];
                        if (arg.isRational()) {
                            double val = double(arg.asRational().numerator()) / arg.asRational().denominator();
                            if (arg.asRational().denominator() % 113 != 0) {
                                val = val * 3.14159265358979323846 / 180.0;
                            }
                            return core::Value(doubleToRational(std::sin(val)));
                        } else if (arg.isComplex() && arg.asComplex().isReal()) {
                            double val = double(arg.asComplex().real().numerator()) / arg.asComplex().real().denominator();
                            if (arg.asComplex().real().denominator() % 113 != 0) {
                                val = val * 3.14159265358979323846 / 180.0;
                            }
                            return core::Value(doubleToRational(std::sin(val)));
                        } else {
                            throw core::MathError("sin expects a real number argument");
                        }
                    }
                    else if (name == "cos") {
                        if (argumentCount != 1) throw core::SyntaxError("cos expects 1 argument");
                        
                        const auto arg = args[0];
                        if (arg.isRational()) {
                            double val = double(arg.asRational().numerator()) / arg.asRational().denominator();
                            if (arg.asRational().denominator() % 113 != 0) {
                                val = val * 3.14159265358979323846 / 180.0;
                            }
                            return core::Value(doubleToRational(std::cos(val)));
                        } else if (arg.isComplex() && arg.asComplex().isReal()) {
                            double val = double(arg.asComplex().real().numerator()) / arg.asComplex().real().denominator();
                            if (arg.asComplex().real().denominator() % 113 != 0) {
                                val = val * 3.14159265358979323846 / 180.0;
                            }
                            return core::Value(doubleToRational(std::cos(val)));
                        } else {
                            throw core::MathError("cos expects a real number argument");
                        }
                    }
                    else if (name == "tan") {
                        if (argumentCount != 1) throw core::SyntaxError("tan expects 1 argument");
                        
                        const auto arg = args[0];
                        if (arg.isRational()) {
                            double val = double(arg.asRational().numerator()) / arg.asRational().denominator();
                            if (arg.asRational().denominator() % 113 != 0) {
                                val = val * 3.14159265358979323846 / 180.0;
                            }
                            return core::Value(doubleToRational(std::tan(val)));
                        } else if (arg.isComplex() && arg.asComplex().isReal()) {
                            double val = double(arg.asComplex().real().numerator()) / arg.asComplex().real().denominator();
                            if (arg.asComplex().real().denominator() % 113 != 0) {
                                val = val * 3.14159265358979323846 / 180.0;
                            }
                            return core::Value(doubleToRational(std::tan(val)));
                        } else {
                            throw core::MathError("tan expects a real number argument");
                        }
                    }
                    else if (name == "log") {
                        if (argumentCount != 1) throw core::SyntaxError("log expects 1 argument");
                        
                        const auto arg = args[0];
                        double val = 0.0;
                        if (arg.isRational()) {
                            val = double(arg.asRational().numerator()) / arg.asRational().denominator();
                        } else if (arg.isComplex() && arg.asComplex().isReal()) {
                            val = double(arg.asComplex().real().numerator()) / arg.asComplex().real().denominator();
                        } else {
                            throw core::MathError("log expects a real number argument");
                        }
                        if (val <= 0.0) {
                            throw core::MathError("log argument must be positive");
                        }
                        return core::Value(doubleToRational(std::log(val)));
                    }
                    else if (name == "exp") {
                        if (argumentCount != 1) throw core::SyntaxError("exp expects 1 argument");
                        
                        const auto arg = args[0];
                        if (arg.isRational()) {
                            double val = double(arg.asRational().numerator()) / arg.asRational().denominator();
                            return core::Value(doubleToRational(std::exp(val)));
                        } else if (arg.isComplex() && arg.asComplex().isReal()) {
                            double val = double(arg.asComplex().real().numerator()) / arg.asComplex().real().denominator();
                            return core::Value(doubleToRational(std::exp(val)));
                        } else {
                            throw core::MathError("exp expects a real number argument");
                        }
                    }
                    else if (name == "abs") {
                        if (argumentCount != 1) throw core::SyntaxError("abs expects 1 argument");
                        
                        const auto arg = args[0];
                        if (arg.isRational()) {
                            auto r = arg.asRational();
                            return core::Value(math::Rational(math::absValue(r.numerator()), r.denominator()));
                        } else if (arg.isComplex()) {
                            auto c = arg.asComplex();
                            auto normSq = c.normSquared();
                            long long n = normSq.numerator();
                            long long d = normSq.denominator();
                            long long sqrt_n = static_cast<long long>(std::round(std::sqrt(n)));
                            long long sqrt_d = static_cast<long long>(std::round(std::sqrt(d)));
                            if (sqrt_n * sqrt_n == n && sqrt_d * sqrt_d == d) {
                                return core::Value(math::Rational(sqrt_n, sqrt_d));
                            } else {
                                double valReal = double(c.real().numerator()) / c.real().denominator();
                                double valImag = double(c.imaginary().numerator()) / c.imaginary().denominator();
                                return core::Value(doubleToRational(std::hypot(valReal, valImag)));
                            }
                        } else {
                            throw core::MathError("abs expects a number argument");
                        }
                    }
                    else if (name == "transpose") {
                        if (argumentCount != 1) throw core::SyntaxError("transpose expects 1 argument");
                        
                        const auto arg = args[0];
                        if (!arg.isMatrix()) throw core::MathError("transpose expects a matrix argument");
                        return core::Value(math::transpose(arg.asMatrix()));
                    }
                    else if (name == "identity") {
                        if (argumentCount != 1) throw core::SyntaxError("identity expects 1 argument");
                        
                        const auto arg = args[0];
                        if (!arg.isRational()) throw core::MathError("identity expects an integer argument");
                        long long val = arg.asRational().numerator();
                        if (arg.asRational().denominator() != 1 || val < 0) throw core::MathError("identity expects a non-negative integer argument");
                        return core::Value(math::identityMatrix(static_cast<std::size_t>(val)));
                    }
                    else if (name == "eval") {
                        if (argumentCount != 2) throw core::SyntaxError("eval expects 2 arguments");
                        
                        const auto poly = args[0];
                        const auto val = args[1];
                        if (!poly.isPolynomial()) throw core::MathError("eval expects a polynomial as the first argument");
                        if (!val.isRational()) throw core::MathError("eval expects a rational number as the second argument");
                        return core::Value(math::evaluatePolynomial(poly.asPolynomial(), val.asRational()));
                    }
                    else if (name == "prime" || name == "isPrime") {
                        if (argumentCount != 1) throw core::SyntaxError("prime expects 1 argument");
                        
                        const auto arg = args[0];
                        if (!arg.isRational()) throw core::MathError("prime expects an integer argument");
                        long long val = arg.asRational().numerator();
                        if (arg.asRational().denominator() != 1) throw core::MathError("prime expects an integer argument");
                        return core::Value(math::Rational(math::prime(val) ? 1 : 0));
                    }
                    else if (name == "modpow") {
                        if (argumentCount != 3) throw core::SyntaxError("modpow expects 3 arguments");
                        
                        const auto base = args[0];
                        const auto exp = args[1];
                        const auto mod = args[2];
                        if (!base.isRational() || !exp.isRational() || !mod.isRational()) throw core::MathError("modpow expects integer arguments");
                        if (base.asRational().denominator() != 1 || exp.asRational().denominator() != 1 || mod.asRational().denominator() != 1) throw core::MathError("modpow expects integer arguments");
                        return core::Value(math::Rational(math::powerModulo(base.asRational().numerator(), exp.asRational().numerator(), mod.asRational().numerator())));
                    }
                    else if (name == "modinv") {
                        if (argumentCount != 2) throw core::SyntaxError("modinv expects 2 arguments");
                        
                        const auto val = args[0];
                        const auto mod = args[1];
                        if (!val.isRational() || !mod.isRational()) throw core::MathError("modinv expects integer arguments");
                        if (val.asRational().denominator() != 1 || mod.asRational().denominator() != 1) throw core::MathError("modinv expects integer arguments");
                        return core::Value(math::Rational(math::inverseModulo(val.asRational().numerator(), mod.asRational().numerator())));
                    }
                    else {
                        throw core::SyntaxError("unknown function: " + name);
                    }
}

}
