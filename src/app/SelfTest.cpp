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
        {"integral({0,2})", "1*x^2"},
        {"sqrt(-1)", "i"},
        {"i * i", "-1"},
        {"(2 + 3*i) * (2 - 3*i)", "13"},
        {"sqrt(-9/16)", "3/4i"},
        {"(9)^(8)", "43046721"},
        {"(1/2)^(-3)", "8"},
        {"(1 + i)^4", "-4"},
        {"{1,2}^2", "1 + 4*x + 4*x^2"},
        {"[1,2;3,4]^2", "[7, 10]\n[15, 22]"},
        {"{1;2}", "1 + 2*x"},
        {"pow(9, 8)", "43046721"},
        {"cos(90)", "0"},
        {"sin(30)", "1/2"},
        {"cos(0)", "1"},
        {"cos(pi)", "-1"},
        {"sin(pi/2)", "1"},
        {"tan(0)", "0"},
        {"log(1)", "0"},
        {"exp(0)", "1"},
        {"abs(-5/2)", "5/2"},
        {"abs(3 + 4*i)", "5"},
        {"transpose([1,2;3,4])", "[1, 3]\n[2, 4]"},
        {"identity(2)", "[1, 0]\n[0, 1]"},
        {"eval({1,2,3}, 2)", "17"},
        {"prime(5)", "1"},
        {"prime(6)", "0"},
        {"modpow(2, 3, 5)", "3"},
        {"modinv(3, 7)", "5"},
        {"5 % 3", "2"},
        {"10 % 3", "1"},
        {"-5 % 3", "-2"}
    };

    const std::vector<ErrorCase> errorCases{
        {""},
        {"2+"},
        {"2/0"},
        {"abc"},
        {"det([1,2,3;4,5,6])"}, // not square
        {"phi(1.5)"}, // not integer
        {"19823712938712983712908371908273901287319023871"},
        {"2^1.5"},
        {"5 % 0"},
        {"1.5 % 2"}
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
