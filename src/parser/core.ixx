module;

#include <optional>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

export module parser.core;

import core.context;
import core.exceptions;
import core.value;

// Закомментированные неиспользуемые математические модули
// import math.complex;
// import math.functions;
// import math.matrix;
// import math.poly;
import math.rational;
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
                    // Заглушка: функции временно отключены
                    throw core::SyntaxError("Functions are not supported yet");
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
                // Заглушка: матрицы временно отключены
                throw core::SyntaxError("Matrix operations are not supported yet");
                continue;
            }

            if (token.kind == TokenKind::Polynomial) {
                // Заглушка: полиномы временно отключены
                throw core::SyntaxError("Polynomial operations are not supported yet");
                continue;
            }

            if (token.kind == TokenKind::Identifier) {
                ensureCanReadOperand(expectsOperand);
                if (isFunctionCall(tokens, index)) {
                    // Заглушка: функции временно отключены
                    throw core::SyntaxError("Functions are not supported yet");
                    continue;
                }

                if (token.lexeme == "i") {
                    // Заглушка: комплексные числа временно отключены
                    throw core::SyntaxError("Complex numbers are not supported yet");
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
        // Парсим всё как Rational
        return {InstructionKind::Number, text, core::Value{math::parseRationalLiteral(text)}};
    }

    Lexer lexer_;
};

}