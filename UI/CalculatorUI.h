#include <SFML/Graphics.hpp>
#include "Button.h"

class CalculatorUI
{
private:
    sf::Font font;
    std::vector<Button> buttonPositions;
    Button helpButton;
    Button bigIntButton;
};