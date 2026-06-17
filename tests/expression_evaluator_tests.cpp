#include "calc/ExpressionEvaluator.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

int failures = 0;

void expectEqual(const std::string& expression, const std::string& expected)
{
    const auto actual = calc::evaluateToString(expression);
    if (actual != expected) {
        std::cerr << "FAIL: " << expression << " expected " << expected << " got " << actual << "\n";
        ++failures;
    }
}

void expectThrows(const std::string& expression)
{
    try {
        (void)calc::evaluateExpression(expression);
        std::cerr << "FAIL: " << expression << " expected exception\n";
        ++failures;
    } catch (const std::exception&) {
    }
}

}

int main()
{
    expectEqual("2+2", "4");
    expectEqual("2+3*4", "14");
    expectEqual("10/4", "2.5");
    expectEqual("-5+2", "-3");
    expectEqual("1.5*2", "3");
    expectEqual("0.1+0.2", "0.3");

    expectThrows("");
    expectThrows("2+");
    expectThrows("2/0");
    expectThrows("abc");

    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return 1;
    }

    std::cout << "all expression evaluator tests passed\n";
    return 0;
}
