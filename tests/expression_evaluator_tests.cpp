#include "app/SelfTest.hpp"

#include <iostream>

int main()
{
    const auto result = app::runSelfTest();

    for (const auto& line : result.log) {
        std::cout << line << "\n";
    }

    std::cout << "selftest: " << result.passed << " passed, " << result.failed << " failed\n";
    return result.ok() ? 0 : 1;
}
