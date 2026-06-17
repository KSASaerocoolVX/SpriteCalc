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
        {"10/4", "2.5"},
        {"-5+2", "-3"},
        {"1.5*2", "3"},
        {"0.1+0.2", "0.3"},
    };

    const std::vector<ErrorCase> errorCases{
        {""},
        {"2+"},
        {"2/0"},
        {"abc"},
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
