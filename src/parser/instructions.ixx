module;

#include <string>
#include <vector>
#include <cstddef>

export module parser.instructions;

import core.value;

export namespace parser {

enum class OperatorKind {
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    Power,
    UnaryMinus,
    LeftParen,
    RightParen
};

enum class InstructionKind {
    Number,
    Variable,
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
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

}
