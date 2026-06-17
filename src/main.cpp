#include "app/Repl.hpp"

#include <iostream>

int main()
{
    app::Repl repl;
    return repl.run(std::cin, std::cout);
}
