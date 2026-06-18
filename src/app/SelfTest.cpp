#include "app/SelfTest.hpp"

#include "calc/ExpressionEvaluator.hpp"

#include <exception>
#include <string>
#include <vector>

namespace app {
namespace {

struct ValueCase {
    std::string expression;
    std::string expected;
};

struct ErrorCase {
    std::string expression;
};

void recordPass(SelfTestResult& result, const std::string& line)
{
    ++result.passed;
    result.log.push_back("PASS " + line);
}

void recordFail(SelfTestResult& result, const std::string& line)
{
    ++result.failed;
    result.log.push_back("FAIL " + line);
}

}

SelfTestResult runSelfTest()
{
    SelfTestResult result;

    const std::vector<ValueCase> valueCases{
        {"2+2", "4"},
        {"2+3*4", "14"},
        {"10/4", "5/2"},
        {"-5+2", "-3"},
        {"1.5*2", "3"},
        {"0.1+0.2", "3/10"},
        {"phi(10)", "4"},
        {"phi(36)", "12"},
        {"gcd(12, 18)", "6"},
        {"lcm(12, 18)", "36"},
        {"det([1,2;3,4])", "-2"},
        {"[1,2;3,4] + [1,1;1,1]", "[2, 3]\n[4, 5]"},
        {"derivative({1,2,3})", "2 + 6*x"},
        {"integral({0,2})", "1*x^2"}
    };

    const std::vector<ErrorCase> errorCases{
        {""},
        {"2+"},
        {"2/0"},
        {"abc"},
        {"det([1,2,3;4,5,6])"}, // not square
        {"phi(1.5)"} // not integer
    };

    for (const auto& test : valueCases) {
        try {
            const auto actual = calc::evaluateToString(test.expression);
            if (actual == test.expected) {
                recordPass(result, test.expression + " = " + actual);
            } else {
                recordFail(result, test.expression + " expected " + test.expected + " got " + actual);
            }
        } catch (const std::exception& error) {
            recordFail(result, test.expression + " threw " + error.what());
        }
    }

    for (const auto& test : errorCases) {
        try {
            (void)calc::evaluateExpression(test.expression);
            recordFail(result, (test.expression.empty() ? "<empty>" : test.expression) + " expected error");
        } catch (const std::exception&) {
            recordPass(result, (test.expression.empty() ? "<empty>" : test.expression) + " throws");
        }
    }

    return result;
}

}
