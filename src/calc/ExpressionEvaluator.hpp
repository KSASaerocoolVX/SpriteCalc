#pragma once

#include <string>

namespace calc {

[[nodiscard]] double evaluateExpression(const std::string& expression);
[[nodiscard]] std::string formatResult(double value);
[[nodiscard]] std::string evaluateToString(const std::string& expression);

}
