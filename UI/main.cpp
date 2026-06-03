#include <SFML/Graphics.hpp>
#include <iostream>
#include "Background.h"
#include "CalculatorUI.h"
using namespace std;

//main потом будет перемещем из Ui

struct Config
{
	unsigned int windowWidth = 800;
	unsigned int windowHeight = 800;
	
	bool enableBg = false;

	string bgShaderPath = "UI/shaders/bg.frag";
};

float easeInOutCubic(float x) {
    return x < 0.5 ? 4 * x * x * x : 1 - std::pow(-2 * x + 2, 3) / 2;
}

bool enable_bg = false;

int main()
{
	Config config;

	float width = config.windowWidth;
	float height = config.windowHeight;

	sf::Vector2 windowCenter = sf::Vector2f(width/2,height/2);

	sf::RenderWindow window(sf::VideoMode({ 800, 800 }), "SpriteCalc");

	//bg
	Background bg(width,height,config.bgShaderPath);

	//calculator
	//принимает только расположение
	CalculatorUI calculator(windowCenter);

	sf::Clock clock;
	float totalTime = 0.f;

	while (window.isOpen())
	{
		sf::Time elasped = clock.restart();
		float deltaTime = elasped.asSeconds();
		totalTime += deltaTime;

		bg.Draw(window);

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
			else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
					window.close();

			}
		}

		window.clear();
		bg.Draw(window, totalTime);
		calculator.Draw(window);
		window.display();
	}
}

