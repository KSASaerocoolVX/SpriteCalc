#include "calc/ExpressionEvaluator.hpp"
#include <stdexcept>
#include <string>

import parser.core;
import core.context;
import core.value;
import math.rational;

namespace calc {

double evaluateExpression(const std::string& expression)
{
    parser::Parser calcParser;
    core::Context ctx;
    auto statement = calcParser.parse(expression);
    core::Value result = statement.evaluate(ctx);
    if (!result.isRational()) {
        throw std::runtime_error("expected a rational number result");
    }
    return static_cast<double>(result.asRational().numerator()) / result.asRational().denominator();
}

std::string formatResult(double value)
{
    // Keeping for compatibility if needed, but not strictly used now
    return std::to_string(value);
}

std::string evaluateToString(const std::string& expression)
{
    parser::Parser calcParser;
    core::Context ctx;
    auto statement = calcParser.parse(expression);
    core::Value result = statement.evaluate(ctx);
    return result.toString();
}

std::string evaluateToString(const std::string& expression, core::Context& ctx)
{
    parser::Parser calcParser;
    auto statement = calcParser.parse(expression);
    core::Value result = statement.evaluate(ctx);
    return result.toString();
}

}
