module;

#include <vector>
#include <optional>
#include <string>
#include <cstddef>

export module parser.evaluator;

import core.context;
import core.exceptions;
import core.value;
import math.dispatcher;
import parser.instructions;

export namespace parser {

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
                    ensureStackSize(stack, instruction.argumentCount);
                    std::vector<core::Value> args(instruction.argumentCount);
                    for (std::size_t i = 0; i < instruction.argumentCount; ++i) {
                        args[instruction.argumentCount - 1 - i] = popValue(stack);
                    }
                    stack.push_back(math::evaluateFunction(instruction.text, args, instruction.argumentCount));
                    break;
                }
                case InstructionKind::Add:
                case InstructionKind::Subtract:
                case InstructionKind::Multiply:
                case InstructionKind::Divide:
                case InstructionKind::Modulo:
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
    void ensureStackSize(const std::vector<core::Value>& stack, std::size_t expected) const {
        if (stack.size() < expected) {
            throw core::SyntaxError("missing operand");
        }
    }

    core::Value popValue(std::vector<core::Value>& stack) const {
        auto value = stack.back();
        stack.pop_back();
        return value;
    }

    void evaluateBinary(InstructionKind kind, std::vector<core::Value>& stack) const {
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
            case InstructionKind::Modulo:
                stack.push_back(core::modulo(left, right));
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

}
