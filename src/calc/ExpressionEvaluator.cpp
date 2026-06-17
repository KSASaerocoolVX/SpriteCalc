#include "calc/ExpressionEvaluator.hpp"

#include <cctype>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace calc {
namespace {

bool isOperator(char value)
{
    return value == '+' || value == '-' || value == '*' || value == '/';
}

int precedence(char operation)
{
    return operation == '*' || operation == '/' ? 2 : 1;
}

void applyOperation(std::vector<double>& values, char operation)
{
    if (values.size() < 2) {
        throw std::runtime_error("missing operand");
    }

    const double right = values.back();
    values.pop_back();
    const double left = values.back();
    values.pop_back();

    switch (operation) {
        case '+':
            values.push_back(left + right);
            break;
        case '-':
            values.push_back(left - right);
            break;
        case '*':
            values.push_back(left * right);
            break;
        case '/':
            if (std::abs(right) <= 1e-12) {
                throw std::runtime_error("division by zero");
            }
            values.push_back(left / right);
            break;
        default:
            throw std::runtime_error("unknown operation");
    }
}

}

double evaluateExpression(const std::string& expression)
{
    std::vector<double> values;
    std::vector<char> operations;
    bool expectsNumber = true;

    for (std::size_t index = 0; index < expression.size();) {
        const char current = expression[index];

        if (std::isspace(static_cast<unsigned char>(current))) {
            ++index;
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(current)) || current == '.' || (current == '-' && expectsNumber)) {
            std::size_t parsed = 0;
            const double value = std::stod(expression.substr(index), &parsed);
            values.push_back(value);
            index += parsed;
            expectsNumber = false;
            continue;
        }

        if (!isOperator(current) || expectsNumber) {
            throw std::runtime_error("invalid expression");
        }

        while (!operations.empty() && precedence(operations.back()) >= precedence(current)) {
            applyOperation(values, operations.back());
            operations.pop_back();
        }

        operations.push_back(current);
        expectsNumber = true;
        ++index;
    }

    if (expectsNumber) {
        throw std::runtime_error(values.empty() ? "empty expression" : "expression ends with operator");
    }

    while (!operations.empty()) {
        applyOperation(values, operations.back());
        operations.pop_back();
    }

    if (values.size() != 1) {
        throw std::runtime_error("invalid expression");
    }

    return values.back();
}

std::string formatResult(double value)
{
    if (std::abs(value) <= 1e-12) {
        value = 0.0;
    }

    std::ostringstream output;
    output.precision(12);
    output << std::fixed << value;

    auto text = output.str();
    while (!text.empty() && text.back() == '0') {
        text.pop_back();
    }
    if (!text.empty() && text.back() == '.') {
        text.pop_back();
    }

    return text.empty() ? "0" : text;
}

std::string evaluateToString(const std::string& expression)
{
    return formatResult(evaluateExpression(expression));
}

}
