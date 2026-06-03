#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window1(sf::VideoMode({ 200, 200 }), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    sf::RenderWindow window2(sf::VideoMode({ 200, 200 }), "SFML works!");

    while (window1.isOpen() && window2.isOpen())
    {
        while (const std::optional event = window1.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window1.close();
        }

        window1.clear();
        window1.draw(shape);
        window1.display();

        window2.clear();
        window2.draw(shape);
        window2.display();
    }
}