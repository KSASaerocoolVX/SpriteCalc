module;

#include <string_view>
#include <vector>
#include <optional>
#include <string>
#include <stdexcept>
#include <cstddef>
#include <algorithm>

export module parser.parser_impl;

import core.context;
import core.exceptions;
import core.value;
import parser.lexer;
import parser.instructions;
import parser.literals;
import parser.evaluator;

export namespace parser {

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
        Modulo,
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
            case TokenKind::Percent:
                if (expectsOperand) {
                    throw core::SyntaxError("unexpected modulo operator");
                }
                return OperatorKind::Modulo;
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
            case OperatorKind::Modulo:
                return {InstructionKind::Modulo, "", {}};
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
            case OperatorKind::Modulo:
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
        return { InstructionKind::Number, text, core::Value(parseDecimalOrInt(text)) };
    }

    Lexer lexer_;
};

inline core::Value evaluate(const std::string& text, core::Context& context) {
    Parser parser;
    auto statement = parser.parse(text);
    return statement.evaluate(context);
}

}
