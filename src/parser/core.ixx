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

export module parser.core;

import core.context;
import core.exceptions;
import core.value;

import math.rational;
import math.matrix;
import math.poly;
import math.number_theory;
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
    Function
};

struct Instruction {
    InstructionKind kind;
    std::string text;
    core::Value value;
    std::size_t argumentCount = 0;
};

// Helper function to parse rational numbers (decimal or integer)
inline math::Rational parseDecimalOrInt(const std::string& text) {
    const auto dot = text.find('.');
    if (dot == std::string::npos) {
        return math::Rational(std::stoll(text));
    }
    std::string whole = text.substr(0, dot);
    std::string frac = text.substr(dot + 1);
    long long wholeVal = whole.empty() ? 0 : std::stoll(whole);
    if (frac.empty()) {
        return math::Rational(wholeVal);
    }
    long long fracVal = std::stoll(frac);
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

// Helper function to parse polynomial literals like {1,2,3}
inline math::Polynomial parsePolynomial(std::string_view text) {
    if (text.size() < 2 || text.front() != '{' || text.back() != '}') {
        throw core::SyntaxError("invalid polynomial syntax");
    }
    std::string_view inner = text.substr(1, text.size() - 2);
    std::vector<math::Rational> coeffs;
    std::size_t idx = 0;
    while (idx < inner.size()) {
        while (idx < inner.size() && (std::isspace(static_cast<unsigned char>(inner[idx])) || inner[idx] == ',')) {
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
                    stack.push_back(context.get(instruction.text));
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
                    else {
                        throw core::SyntaxError("unknown function: " + instruction.text);
                    }
                    break;
                }
                case InstructionKind::Add:
                case InstructionKind::Subtract:
                case InstructionKind::Multiply:
                case InstructionKind::Divide:
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
        return operation == OperatorKind::UnaryMinus || operation == OperatorKind::Function;
    }

    [[nodiscard]] Instruction makeNumberInstruction(const std::string& text) const {
        return { InstructionKind::Number, text, core::Value(parseDecimalOrInt(text)) };
    }

    Lexer lexer_;
};

}
