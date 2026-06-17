#pragma once

#include <iosfwd>

namespace app {

class Repl {
public:
    int run(std::istream& input, std::ostream& output);
};

}
