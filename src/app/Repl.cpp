#include "app/Repl.hpp"

#include "app/SelfTest.hpp"
#include "calc/ExpressionEvaluator.hpp"

#include <chrono>
#include <exception>
#include <istream>
#include <ostream>
#include <string>
#include <string_view>

namespace app {
namespace {

std::string trim(std::string_view text)
{
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos) {
        return {};
    }

    const auto last = text.find_last_not_of(" \t\r\n");
    return std::string{text.substr(first, last - first + 1)};
}

bool startsWith(std::string_view text, std::string_view prefix)
{
    return text.size() >= prefix.size() && text.substr(0, prefix.size()) == prefix;
}

bool isExitCommand(std::string_view input)
{
    return input == "q" || input == "quit" || input == "exit";
}

void printHelp(std::ostream& output)
{
    output
        << "AsciCalc / SpriteCalc console REPL\n"
        << "\n"
        << "Supported now:\n"
        << "  2 + 2             arithmetic\n"
        << "  1 / 3 + 1 / 6     expression input with operator precedence\n"
        << "  -5 + 2            negative numbers\n"
        << "  1.5 * 2           decimal numbers\n"
        << "  percent 50        percent helper, returns 0.5\n"
        << "\n"
        << "Commands:\n"
        << "  help              show this help\n"
        << "  vars              show stored variables\n"
        << "  selftest          run built-in tests\n"
        << "  test, tests       aliases for selftest\n"
        << "  bench             run a tiny evaluator benchmark\n"
        << "  clear             clear the last result\n"
        << "  q, quit, exit     quit\n"
        << "\n"
        << "Planned by TZ.md, not enabled in this build yet:\n"
        << "  phi(10), det([1,2;3,4]), matrices, polynomials, derivatives, integrals\n"
        << "\n";
}

void printSelfTest(std::ostream& output)
{
    const auto result = runSelfTest();

    for (const auto& line : result.log) {
        output << line << "\n";
    }

    output << "selftest: " << result.passed << " passed, " << result.failed << " failed\n";
}

void runBench(std::ostream& output)
{
    constexpr int iterations = 10000;
    const auto start = std::chrono::steady_clock::now();

    std::string result;
    for (int index = 0; index < iterations; ++index) {
        result = calc::evaluateToString("2+3*4-10/5");
    }

    const auto finish = std::chrono::steady_clock::now();
    const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    output << "bench: " << iterations << " expressions in " << elapsed.count()
           << " ms, last result = " << result << "\n";
}

}

int Repl::run(std::istream& input, std::ostream& output)
{
    std::string line;
    std::string lastResult = "0";

    printHelp(output);

    while (true) {
        output << "calc> ";
        if (!std::getline(input, line)) {
            break;
        }

        line = trim(line);

        if (isExitCommand(line)) {
            break;
        }

        if (line == "help") {
            printHelp(output);
            continue;
        }

        if (line == "vars") {
            output << "vars: no variables are stored yet\n";
            continue;
        }

        if (line == "selftest" || line == "test" || line == "tests") {
            printSelfTest(output);
            continue;
        }

        if (line == "bench") {
            runBench(output);
            continue;
        }

        if (line == "clear" || line == "AC" || line == "ac") {
            lastResult = "0";
            output << lastResult << "\n";
            continue;
        }

        try {
            if (startsWith(line, "percent ")) {
                const auto expression = trim(std::string_view{line}.substr(8));
                lastResult = calc::formatResult(calc::evaluateExpression(expression) / 100.0);
            } else {
                lastResult = calc::evaluateToString(line);
            }

            output << lastResult << "\n";
        } catch (const std::exception& error) {
            output << "error: " << error.what() << "\n";
        }
    }

    return 0;
}

}
