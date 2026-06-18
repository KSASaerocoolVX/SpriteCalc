#pragma once

#include <string>
#include <vector>

namespace app {

struct SelfTestResult {
    int passed = 0;
    int failed = 0;
    std::vector<std::string> log;

    [[nodiscard]] bool ok() const noexcept {
        return failed == 0;
    }
};

[[nodiscard]] SelfTestResult runSelfTest();

}
