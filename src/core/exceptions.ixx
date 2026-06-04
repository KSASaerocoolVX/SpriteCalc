// ЗАГЛУШКА

module;

#include <stdexcept>
#include <string>
#include <utility>

export module core.exceptions;

export namespace core {

class CalcError : public std::runtime_error {
public:
    explicit CalcError(std::string message)
        : std::runtime_error(std::move(message)) {}
};

class MathError final : public CalcError {
public:
    explicit MathError(std::string message)
        : CalcError(std::move(message)) {}
};

class SyntaxError final : public CalcError {
public:
    explicit SyntaxError(std::string message)
        : CalcError(std::move(message)) {}
};

}