module;

#include <optional>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <cmath>

export module parser.core;

import core.context;
import core.exceptions;
import core.value;

import math.rational;
import math.matrix;
import math.poly;
import math.number_theory;
import math.complex;
import math.functions;
import parser.lexer;

export namespace parser {

enum class InstructionKind {
    Number,
    Variable,
    Add,
    Subtract,
    Multiply,
    Divide,
    Negate,
    Function,
    Power
};

struct Instruction {
    InstructionKind kind;
    std::string text;
    core::Value value;
    std::size_t argumentCount = 0;
};

// Helper function to parse rational numbers (decimal or integer)
inline math::Rational parseDecimalOrInt(const std::string& text) {
    if (text.empty()) {
        throw core::SyntaxError("empty number literal");
    }
    const auto dot = text.find('.');
    if (dot == std::string::npos) {
        try {
            return math::Rational(std::stoll(text));
        } catch (...) {
            throw core::SyntaxError("integer literal is too large or invalid: " + text);
        }
    }
    std::string whole = text.substr(0, dot);
    std::string frac = text.substr(dot + 1);
    long long wholeVal = 0;
    if (!whole.empty()) {
        try {
            wholeVal = std::stoll(whole);
        } catch (...) {
            throw core::SyntaxError("invalid integer literal in decimal: " + whole);
        }
    }
    if (frac.empty()) {
        return math::Rational(wholeVal);
    }
    long long fracVal = 0;
    try {
        fracVal = std::stoll(frac);
    } catch (...) {
        throw core::SyntaxError("invalid fractional literal in decimal: " + frac);
    }
    long long denom = 1;
    for (std::size_t i = 0; i < frac.size(); ++i) {
        denom *= 10;
    }
    math::Rational w(wholeVal);
    math::Rational f(fracVal, denom);
    return wholeVal >= 0 ? w + f : w - f;
}

// Helper function to parse matrix literals like [1,2;3,4]
inline math::Matrix parseMatrix(std::string_view text) {
    if (text.size() < 2 || text.front() != '[' || text.back() != ']') {
        throw core::SyntaxError("invalid matrix syntax");
    }
    std::string_view inner = text.substr(1, text.size() - 2);
    std::vector<std::vector<math::Rational>> rows;
    std::size_t start = 0;
    while (start < inner.size()) {
        auto end = inner.find(';', start);
        if (end == std::string_view::npos) {
            end = inner.size();
        }
        std::string_view row_text = inner.substr(start, end - start);
        std::vector<math::Rational> row;
        std::size_t idx = 0;
        while (idx < row_text.size()) {
            while (idx < row_text.size() && (std::isspace(static_cast<unsigned char>(row_text[idx])) || row_text[idx] == ',')) {
                ++idx;
            }
            if (idx >= row_text.size()) break;
            std::size_t num_start = idx;
            if (row_text[idx] == '-') {
                ++idx;
            }
            while (idx < row_text.size() && (std::isdigit(static_cast<unsigned char>(row_text[idx])) || row_text[idx] == '.')) {
                ++idx;
            }
            if (num_start == idx || (idx - num_start == 1 && row_text[num_start] == '-')) {
                throw core::SyntaxError("invalid number in matrix literal: " + std::string(row_text.substr(num_start, 1)));
            }
            std::string num_str{row_text.substr(num_start, idx - num_start)};
            row.push_back(parseDecimalOrInt(num_str));
        }
        if (!row.empty()) {
            rows.push_back(std::move(row));
        }
        start = end + 1;
    }
    if (rows.empty()) {
        throw core::SyntaxError("empty matrix");
    }
    std::size_t row_count = rows.size();
    std::size_t col_count = rows[0].size();
    for (const auto& r : rows) {
        if (r.size() != col_count) {
            throw core::SyntaxError("matrix rows must have the same number of columns");
        }
    }
    math::Matrix m(row_count, col_count);
    for (std::size_t r = 0; r < row_count; ++r) {
        for (std::size_t c = 0; c < col_count; ++c) {
            m.at(r, c) = rows[r][c];
        }
    }
    return m;
}

// Helper function to parse polynomial literals like {1,2,3} or {1;2}
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

inline math::Polynomial parsePolynomial(std::string_view text) {
    if (text.size() < 2 || text.front() != '{' || text.back() != '}') {
        throw core::SyntaxError("invalid polynomial syntax");
    }
    std::string_view inner = text.substr(1, text.size() - 2);
    std::vector<math::Rational> coeffs;
    std::size_t idx = 0;
    while (idx < inner.size()) {
        while (idx < inner.size() && (std::isspace(static_cast<unsigned char>(inner[idx])) || inner[idx] == ',' || inner[idx] == ';')) {
            ++idx;
        }
        if (idx >= inner.size()) break;
        std::size_t num_start = idx;
        if (inner[idx] == '-') {
            ++idx;
        }
        while (idx < inner.size() && (std::isdigit(static_cast<unsigned char>(inner[idx])) || inner[idx] == '.')) {
            ++idx;
        }
        if (num_start == idx || (idx - num_start == 1 && inner[num_start] == '-')) {
            throw core::SyntaxError("invalid number in polynomial literal: " + std::string(inner.substr(num_start, 1)));
        }
        std::string num_str{inner.substr(num_start, idx - num_start)};
        coeffs.push_back(parseDecimalOrInt(num_str));
    }
    return math::Polynomial(std::move(coeffs));
}
class Statement {
public:
    Statement(std::optional<std::string> assignmentName, std::vector<Instruction> instructions)
        : assignmentName_(std::move(assignmentName)),
          instructions_(std::move(instructions)) {}

    [[nodiscard]] core::Value evaluate(core::Context& context) const {
        std::vector<core::Value> stack;

        for (const auto& instruction : instructions_) {
            switch (instruction.kind) {
                case InstructionKind::Number:
                    stack.push_back(instruction.value);
                    break;
                case InstructionKind::Variable:
                    if (instruction.text == "i") {
                        stack.push_back(core::Value(math::Complex(math::Rational(0), math::Rational(1))));
                    } else if (instruction.text == "pi") {
                        stack.push_back(core::Value(math::Rational(355, 113)));
                    } else {
                        stack.push_back(context.get(instruction.text));
                    }
                    break;
                case InstructionKind::Negate: {
                    ensureStackSize(stack, 1);
                    const auto value = popValue(stack);
                    stack.push_back(core::negate(value));
                    break;
                }
                case InstructionKind::Function: {
                    if (instruction.text == "phi") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("phi expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
                        if (!arg.isRational()) throw core::MathError("phi expects an integer argument");
                        long long val = arg.asRational().numerator();
                        if (arg.asRational().denominator() != 1) throw core::MathError("phi expects an integer argument");
                        stack.push_back(core::Value(math::Rational(math::phi(val))));
                    }
                    else if (instruction.text == "det") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("det expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
                        if (!arg.isMatrix()) throw core::MathError("det expects a matrix argument");
                        stack.push_back(core::Value(arg.asMatrix().determinant()));
                    }
                    else if (instruction.text == "derivative") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("derivative expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
                        if (!arg.isPolynomial()) throw core::MathError("derivative expects a polynomial argument");
                        stack.push_back(core::Value(arg.asPolynomial().derivative()));
                    }
                    else if (instruction.text == "integral") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("integral expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
                        if (!arg.isPolynomial()) throw core::MathError("integral expects a polynomial argument");
                        stack.push_back(core::Value(arg.asPolynomial().integral()));
                    }
                    else if (instruction.text == "gcd") {
                        if (instruction.argumentCount != 2) throw core::SyntaxError("gcd expects 2 arguments");
                        ensureStackSize(stack, 2);
                        const auto right = popValue(stack);
                        const auto left = popValue(stack);
                        if (!left.isRational() || !right.isRational()) throw core::MathError("gcd expects integer arguments");
                        if (left.asRational().denominator() != 1 || right.asRational().denominator() != 1) throw core::MathError("gcd expects integer arguments");
                        stack.push_back(core::Value(math::Rational(math::gcd(left.asRational().numerator(), right.asRational().numerator()))));
                    }
                    else if (instruction.text == "lcm") {
                        if (instruction.argumentCount != 2) throw core::SyntaxError("lcm expects 2 arguments");
                        ensureStackSize(stack, 2);
                        const auto right = popValue(stack);
                        const auto left = popValue(stack);
                        if (!left.isRational() || !right.isRational()) throw core::MathError("lcm expects integer arguments");
                        if (left.asRational().denominator() != 1 || right.asRational().denominator() != 1) throw core::MathError("lcm expects integer arguments");
                        stack.push_back(core::Value(math::Rational(math::lcm(left.asRational().numerator(), right.asRational().numerator()))));
                    }
                    else if (instruction.text == "sqrt") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("sqrt expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
                        if (arg.isRational()) {
                            auto r = arg.asRational();
                            long long n = r.numerator();
                            long long d = r.denominator();
                            if (n >= 0) {
                                long long sqrt_n = static_cast<long long>(std::round(std::sqrt(n)));
                                long long sqrt_d = static_cast<long long>(std::round(std::sqrt(d)));
                                if (sqrt_n * sqrt_n == n && sqrt_d * sqrt_d == d) {
                                    stack.push_back(core::Value(math::Rational(sqrt_n, sqrt_d)));
                                } else {
                                    throw core::MathError("exact square root is not rational");
                                }
                            } else {
                                long long pos_n = -n;
                                long long sqrt_n = static_cast<long long>(std::round(std::sqrt(pos_n)));
                                long long sqrt_d = static_cast<long long>(std::round(std::sqrt(d)));
                                if (sqrt_n * sqrt_n == pos_n && sqrt_d * sqrt_d == d) {
                                    stack.push_back(core::Value(math::Complex(math::Rational(0), math::Rational(sqrt_n, sqrt_d))));
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
                                        stack.push_back(core::Value(math::Rational(sqrt_n, sqrt_d)));
                                    } else {
                                        throw core::MathError("exact square root is not rational");
                                    }
                                } else {
                                    long long pos_n = -n;
                                    long long sqrt_n = static_cast<long long>(std::round(std::sqrt(pos_n)));
                                    long long sqrt_d = static_cast<long long>(std::round(std::sqrt(d)));
                                    if (sqrt_n * sqrt_n == pos_n && sqrt_d * sqrt_d == d) {
                                        stack.push_back(core::Value(math::Complex(math::Rational(0), math::Rational(sqrt_n, sqrt_d))));
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
                    else if (instruction.text == "pow") {
                        if (instruction.argumentCount != 2) throw core::SyntaxError("pow expects 2 arguments");
                        ensureStackSize(stack, 2);
                        const auto right = popValue(stack);
                        const auto left = popValue(stack);
                        stack.push_back(core::power(left, right));
                    }
                    else if (instruction.text == "sin") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("sin expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
                        if (arg.isRational()) {
                            double val = double(arg.asRational().numerator()) / arg.asRational().denominator();
                            if (arg.asRational().denominator() % 113 != 0) {
                                val = val * 3.14159265358979323846 / 180.0;
                            }
                            stack.push_back(core::Value(doubleToRational(std::sin(val))));
                        } else if (arg.isComplex() && arg.asComplex().isReal()) {
                            double val = double(arg.asComplex().real().numerator()) / arg.asComplex().real().denominator();
                            if (arg.asComplex().real().denominator() % 113 != 0) {
                                val = val * 3.14159265358979323846 / 180.0;
                            }
                            stack.push_back(core::Value(doubleToRational(std::sin(val))));
                        } else {
                            throw core::MathError("sin expects a real number argument");
                        }
                    }
                    else if (instruction.text == "cos") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("cos expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
                        if (arg.isRational()) {
                            double val = double(arg.asRational().numerator()) / arg.asRational().denominator();
                            if (arg.asRational().denominator() % 113 != 0) {
                                val = val * 3.14159265358979323846 / 180.0;
                            }
                            stack.push_back(core::Value(doubleToRational(std::cos(val))));
                        } else if (arg.isComplex() && arg.asComplex().isReal()) {
                            double val = double(arg.asComplex().real().numerator()) / arg.asComplex().real().denominator();
                            if (arg.asComplex().real().denominator() % 113 != 0) {
                                val = val * 3.14159265358979323846 / 180.0;
                            }
                            stack.push_back(core::Value(doubleToRational(std::cos(val))));
                        } else {
                            throw core::MathError("cos expects a real number argument");
                        }
                    }
                    else if (instruction.text == "tan") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("tan expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
                        if (arg.isRational()) {
                            double val = double(arg.asRational().numerator()) / arg.asRational().denominator();
                            if (arg.asRational().denominator() % 113 != 0) {
                                val = val * 3.14159265358979323846 / 180.0;
                            }
                            stack.push_back(core::Value(doubleToRational(std::tan(val))));
                        } else if (arg.isComplex() && arg.asComplex().isReal()) {
                            double val = double(arg.asComplex().real().numerator()) / arg.asComplex().real().denominator();
                            if (arg.asComplex().real().denominator() % 113 != 0) {
                                val = val * 3.14159265358979323846 / 180.0;
                            }
                            stack.push_back(core::Value(doubleToRational(std::tan(val))));
                        } else {
                            throw core::MathError("tan expects a real number argument");
                        }
                    }
                    }
                    else if (instruction.text == "log") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("log expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
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
                        stack.push_back(core::Value(doubleToRational(std::log(val))));
                    }
                    else if (instruction.text == "exp") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("exp expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
                        if (arg.isRational()) {
                            double val = double(arg.asRational().numerator()) / arg.asRational().denominator();
                            stack.push_back(core::Value(doubleToRational(std::exp(val))));
                        } else if (arg.isComplex() && arg.asComplex().isReal()) {
                            double val = double(arg.asComplex().real().numerator()) / arg.asComplex().real().denominator();
                            stack.push_back(core::Value(doubleToRational(std::exp(val))));
                        } else {
                            throw core::MathError("exp expects a real number argument");
                        }
                    }
                    else if (instruction.text == "abs") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("abs expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
                        if (arg.isRational()) {
                            auto r = arg.asRational();
                            stack.push_back(core::Value(math::Rational(math::absValue(r.numerator()), r.denominator())));
                        } else if (arg.isComplex()) {
                            auto c = arg.asComplex();
                            auto normSq = c.normSquared();
                            long long n = normSq.numerator();
                            long long d = normSq.denominator();
                            long long sqrt_n = static_cast<long long>(std::round(std::sqrt(n)));
                            long long sqrt_d = static_cast<long long>(std::round(std::sqrt(d)));
                            if (sqrt_n * sqrt_n == n && sqrt_d * sqrt_d == d) {
                                stack.push_back(core::Value(math::Rational(sqrt_n, sqrt_d)));
                            } else {
                                double valReal = double(c.real().numerator()) / c.real().denominator();
                                double valImag = double(c.imaginary().numerator()) / c.imaginary().denominator();
                                stack.push_back(core::Value(doubleToRational(std::hypot(valReal, valImag))));
                            }
                        } else {
                            throw core::MathError("abs expects a number argument");
                        }
                    }
                    else if (instruction.text == "transpose") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("transpose expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
                        if (!arg.isMatrix()) throw core::MathError("transpose expects a matrix argument");
                        stack.push_back(core::Value(math::transpose(arg.asMatrix())));
                    }
                    else if (instruction.text == "identity") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("identity expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
                        if (!arg.isRational()) throw core::MathError("identity expects an integer argument");
                        long long val = arg.asRational().numerator();
                        if (arg.asRational().denominator() != 1 || val < 0) throw core::MathError("identity expects a non-negative integer argument");
                        stack.push_back(core::Value(math::identityMatrix(static_cast<std::size_t>(val))));
                    }
                    else if (instruction.text == "eval") {
                        if (instruction.argumentCount != 2) throw core::SyntaxError("eval expects 2 arguments");
                        ensureStackSize(stack, 2);
                        const auto val = popValue(stack);
                        const auto poly = popValue(stack);
                        if (!poly.isPolynomial()) throw core::MathError("eval expects a polynomial as the first argument");
                        if (!val.isRational()) throw core::MathError("eval expects a rational number as the second argument");
                        stack.push_back(core::Value(math::evaluatePolynomial(poly.asPolynomial(), val.asRational())));
                    }
                    else if (instruction.text == "prime" || instruction.text == "isPrime") {
                        if (instruction.argumentCount != 1) throw core::SyntaxError("prime expects 1 argument");
                        ensureStackSize(stack, 1);
                        const auto arg = popValue(stack);
                        if (!arg.isRational()) throw core::MathError("prime expects an integer argument");
                        long long val = arg.asRational().numerator();
                        if (arg.asRational().denominator() != 1) throw core::MathError("prime expects an integer argument");
                        stack.push_back(core::Value(math::Rational(math::prime(val) ? 1 : 0)));
                    }
                    else if (instruction.text == "modpow") {
                        if (instruction.argumentCount != 3) throw core::SyntaxError("modpow expects 3 arguments");
                        ensureStackSize(stack, 3);
                        const auto mod = popValue(stack);
                        const auto exp = popValue(stack);
                        const auto base = popValue(stack);
                        if (!base.isRational() || !exp.isRational() || !mod.isRational()) throw core::MathError("modpow expects integer arguments");
                        if (base.asRational().denominator() != 1 || exp.asRational().denominator() != 1 || mod.asRational().denominator() != 1) throw core::MathError("modpow expects integer arguments");
                        stack.push_back(core::Value(math::Rational(math::powerModulo(base.asRational().numerator(), exp.asRational().numerator(), mod.asRational().numerator()))));
                    }
                    else if (instruction.text == "modinv") {
                        if (instruction.argumentCount != 2) throw core::SyntaxError("modinv expects 2 arguments");
                        ensureStackSize(stack, 2);
                        const auto mod = popValue(stack);
                        const auto val = popValue(stack);
                        if (!val.isRational() || !mod.isRational()) throw core::MathError("modinv expects integer arguments");
                        if (val.asRational().denominator() != 1 || mod.asRational().denominator() != 1) throw core::MathError("modinv expects integer arguments");
                        stack.push_back(core::Value(math::Rational(math::inverseModulo(val.asRational().numerator(), mod.asRational().numerator()))));
                    }
                    else {
                        throw core::SyntaxError("unknown function: " + instruction.text);
                    }
                    break;
                }
                case InstructionKind::Add:
                case InstructionKind::Subtract:
                case InstructionKind::Multiply:
                case InstructionKind::Divide:
                case InstructionKind::Power:
                    evaluateBinary(instruction.kind, stack);
                    break;
            }
        }

        if (stack.size() != 1) {
            throw core::SyntaxError("invalid expression");
        }

        auto result = popValue(stack);
        if (assignmentName_.has_value()) {
            context.set(*assignmentName_, result);
        }

        return result;
    }

private:
    static void ensureStackSize(const std::vector<core::Value>& stack, std::size_t expected) {
        if (stack.size() < expected) {
            throw core::SyntaxError("missing operand");
        }
    }

    static core::Value popValue(std::vector<core::Value>& stack) {
        auto value = stack.back();
        stack.pop_back();
        return value;
    }

    static void evaluateBinary(InstructionKind kind, std::vector<core::Value>& stack) {
        ensureStackSize(stack, 2);

        const auto right = popValue(stack);
        const auto left = popValue(stack);

        switch (kind) {
            case InstructionKind::Add:
                stack.push_back(core::add(left, right));
                break;
            case InstructionKind::Subtract:
                stack.push_back(core::subtract(left, right));
                break;
            case InstructionKind::Multiply:
                stack.push_back(core::multiply(left, right));
                break;
            case InstructionKind::Divide:
                stack.push_back(core::divide(left, right));
                break;
            case InstructionKind::Power:
                stack.push_back(core::power(left, right));
                break;
            default:
                throw core::SyntaxError("expected binary instruction");
        }
    }

    std::optional<std::string> assignmentName_;
    std::vector<Instruction> instructions_;
};

class Parser {
public:
    [[nodiscard]] Statement parse(std::string_view input) const {
        const auto tokens = lexer_.tokenize(input);

        if (tokens.size() == 1 && tokens.front().kind == TokenKind::End) {
            throw core::SyntaxError("empty expression");
        }

        std::optional<std::string> assignmentName;
        std::size_t expressionStart = 0;

        if (
            tokens.size() >= 3 &&
            tokens[0].kind == TokenKind::Identifier &&
            tokens[1].kind == TokenKind::Equal
        ) {
            assignmentName = std::string{tokens[0].lexeme};
            expressionStart = 2;
        }

        return Statement{std::move(assignmentName), toPostfix(tokens, expressionStart)};
    }

private:
    enum class OperatorKind {
        Add,
        Subtract,
        Multiply,
        Divide,
        Power,
        UnaryMinus,
        Function,
        LeftParen
    };

    struct FunctionState {
        std::string name;
        std::size_t completedArguments = 0;
        bool hasCurrentArgument = false;
    };

    [[nodiscard]] std::vector<Instruction> toPostfix(const std::vector<Token>& tokens, std::size_t start) const {
        std::vector<Instruction> output;
        std::vector<OperatorKind> operators;
        std::vector<FunctionState> functions;
        bool expectsOperand = true;

        for (std::size_t index = start; index < tokens.size(); ++index) {
            const auto& token = tokens[index];

            if (token.kind == TokenKind::End) {
                break;
            }

            if (token.kind == TokenKind::Number) {
                ensureCanReadOperand(expectsOperand);
                output.push_back(makeNumberInstruction(std::string{token.lexeme}));
                markCurrentFunctionArgument(functions);
                expectsOperand = false;
                continue;
            }

            if (token.kind == TokenKind::Matrix) {
                ensureCanReadOperand(expectsOperand);
                output.push_back({InstructionKind::Number, std::string{token.lexeme}, core::Value(parseMatrix(token.lexeme))});
                markCurrentFunctionArgument(functions);
                expectsOperand = false;
                continue;
            }

            if (token.kind == TokenKind::Polynomial) {
                ensureCanReadOperand(expectsOperand);
                output.push_back({InstructionKind::Number, std::string{token.lexeme}, core::Value(parsePolynomial(token.lexeme))});
                markCurrentFunctionArgument(functions);
                expectsOperand = false;
                continue;
            }

            if (token.kind == TokenKind::Identifier) {
                ensureCanReadOperand(expectsOperand);
                if (isFunctionCall(tokens, index)) {
                    operators.push_back(OperatorKind::Function);
                    functions.push_back({std::string{token.lexeme}, 0, false});
                    continue;
                }

                output.push_back({InstructionKind::Variable, std::string{token.lexeme}, {}});
                markCurrentFunctionArgument(functions);
                expectsOperand = false;
                continue;
            }

            if (token.kind == TokenKind::LeftParen) {
                operators.push_back(OperatorKind::LeftParen);
                expectsOperand = true;
                continue;
            }

            if (token.kind == TokenKind::RightParen) {
                if (expectsOperand && !isClosingEmptyFunction(operators, functions)) {
                    throw core::SyntaxError("missing function argument");
                }

                closeParenthesis(output, operators, functions);
                markCurrentFunctionArgument(functions);
                expectsOperand = false;
                continue;
            }

            if (token.kind == TokenKind::Comma) {
                if (expectsOperand) {
                    throw core::SyntaxError("missing function argument");
                }

                closeFunctionArgument(output, operators, functions);
                expectsOperand = true;
                continue;
            }

            if (token.kind == TokenKind::Equal) {
                throw core::SyntaxError("unexpected assignment operator");
            }

            const auto op = tokenToOperator(token, expectsOperand);
            pushOperator(op, output, operators, functions);
            expectsOperand = true;
        }

        if (expectsOperand && !output.empty()) {
            throw core::SyntaxError("expression ends with an operator");
        }

        while (!operators.empty()) {
            if (operators.back() == OperatorKind::LeftParen) {
                throw core::SyntaxError("unclosed parenthesis");
            }

            output.push_back(toInstruction(operators.back(), functions));
            operators.pop_back();
        }

        if (output.empty()) {
            throw core::SyntaxError("empty expression");
        }

        return output;
    }

    void pushOperator(
        OperatorKind incoming,
        std::vector<Instruction>& output,
        std::vector<OperatorKind>& operators,
        std::vector<FunctionState>& functions
    ) const {
        while (!operators.empty() && operators.back() != OperatorKind::LeftParen) {
            const auto top = operators.back();
            const auto shouldPop = isRightAssociative(incoming)
                ? precedence(incoming) < precedence(top)
                : precedence(incoming) <= precedence(top);

            if (!shouldPop) {
                break;
            }

            output.push_back(toInstruction(top, functions));
            operators.pop_back();
        }

        operators.push_back(incoming);
    }

    void closeParenthesis(
        std::vector<Instruction>& output,
        std::vector<OperatorKind>& operators,
        std::vector<FunctionState>& functions
    ) const {
        while (!operators.empty() && operators.back() != OperatorKind::LeftParen) {
            output.push_back(toInstruction(operators.back(), functions));
            operators.pop_back();
        }

        if (operators.empty()) {
            throw core::SyntaxError("unexpected closing parenthesis");
        }

        operators.pop_back();

        if (!operators.empty() && operators.back() == OperatorKind::Function) {
            output.push_back(toInstruction(operators.back(), functions));
            operators.pop_back();
        }
    }

    void closeFunctionArgument(
        std::vector<Instruction>& output,
        std::vector<OperatorKind>& operators,
        std::vector<FunctionState>& functions
    ) const {
        while (!operators.empty() && operators.back() != OperatorKind::LeftParen) {
            output.push_back(toInstruction(operators.back(), functions));
            operators.pop_back();
        }

        if (operators.empty() || !isFunctionParenthesis(operators)) {
            throw core::SyntaxError("comma is only valid inside function arguments");
        }

        if (functions.empty() || !functions.back().hasCurrentArgument) {
            throw core::SyntaxError("missing function argument");
        }

        ++functions.back().completedArguments;
        functions.back().hasCurrentArgument = false;
    }

    [[nodiscard]] bool isFunctionCall(const std::vector<Token>& tokens, std::size_t index) const {
        return index + 1 < tokens.size() && tokens[index + 1].kind == TokenKind::LeftParen;
    }

    void ensureCanReadOperand(bool expectsOperand) const {
        if (!expectsOperand) {
            throw core::SyntaxError("missing operator");
        }
    }

    [[nodiscard]] OperatorKind tokenToOperator(const Token& token, bool expectsOperand) const {
        switch (token.kind) {
            case TokenKind::Plus:
                if (expectsOperand) {
                    throw core::SyntaxError("unexpected plus operator");
                }
                return OperatorKind::Add;
            case TokenKind::Minus:
                return expectsOperand ? OperatorKind::UnaryMinus : OperatorKind::Subtract;
            case TokenKind::Star:
                if (expectsOperand) {
                    throw core::SyntaxError("unexpected multiplication operator");
                }
                return OperatorKind::Multiply;
            case TokenKind::Slash:
                if (expectsOperand) {
                    throw core::SyntaxError("unexpected division operator");
                }
                return OperatorKind::Divide;
            case TokenKind::Caret:
                if (expectsOperand) {
                    throw core::SyntaxError("unexpected power operator");
                }
                return OperatorKind::Power;
            default:
                throw core::SyntaxError("expected operator");
        }
    }

    [[nodiscard]] Instruction toInstruction(OperatorKind operation, std::vector<FunctionState>& functions) const {
        switch (operation) {
            case OperatorKind::Add:
                return {InstructionKind::Add, "", {}};
            case OperatorKind::Subtract:
                return {InstructionKind::Subtract, "", {}};
            case OperatorKind::Multiply:
                return {InstructionKind::Multiply, "", {}};
            case OperatorKind::Divide:
                return {InstructionKind::Divide, "", {}};
            case OperatorKind::Power:
                return {InstructionKind::Power, "", {}};
            case OperatorKind::UnaryMinus:
                return {InstructionKind::Negate, "", {}};
            case OperatorKind::Function: {
                if (functions.empty()) {
                    throw core::SyntaxError("function stack is empty");
                }

                const auto state = functions.back();
                functions.pop_back();
                const auto argumentCount = state.hasCurrentArgument ? state.completedArguments + 1 : 0;
                return {InstructionKind::Function, state.name, {}, argumentCount};
            }
            case OperatorKind::LeftParen:
                break;
        }

        throw core::SyntaxError("cannot emit parenthesis instruction");
    }

    void markCurrentFunctionArgument(std::vector<FunctionState>& functions) const {
        if (!functions.empty()) {
            functions.back().hasCurrentArgument = true;
        }
    }

    [[nodiscard]] bool isFunctionParenthesis(const std::vector<OperatorKind>& operators) const {
        return operators.size() >= 2 &&
            operators.back() == OperatorKind::LeftParen &&
            operators[operators.size() - 2] == OperatorKind::Function;
    }

    [[nodiscard]] bool isClosingEmptyFunction(
        const std::vector<OperatorKind>& operators,
        const std::vector<FunctionState>& functions
    ) const {
        return isFunctionParenthesis(operators) &&
            !functions.empty() &&
            !functions.back().hasCurrentArgument &&
            functions.back().completedArguments == 0;
    }

    [[nodiscard]] int precedence(OperatorKind operation) const {
        switch (operation) {
            case OperatorKind::UnaryMinus:
            case OperatorKind::Power:
                return 4;
            case OperatorKind::Function:
                return 3;
            case OperatorKind::Multiply:
            case OperatorKind::Divide:
                return 2;
            case OperatorKind::Add:
            case OperatorKind::Subtract:
                return 1;
            case OperatorKind::LeftParen:
                return 0;
        }

        return 0;
    }

    [[nodiscard]] bool isRightAssociative(OperatorKind operation) const {
        return operation == OperatorKind::UnaryMinus || operation == OperatorKind::Function || operation == OperatorKind::Power;
    }

    [[nodiscard]] Instruction makeNumberInstruction(const std::string& text) const {
<<<<<<< HEAD
        return { InstructionKind::Number, text, core::Value(parseDecimalOrInt(text)) };
=======
        return { InstructionKind::Number, text, core::Value(math::Rational(std::stoll(text))) };
>>>>>>> main
    }

    Lexer lexer_;
};

}

