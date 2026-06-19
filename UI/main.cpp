#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <cmath>

import Background;
import CalculatorUI;

using namespace std;

//main потом будет перемещем из Ui

struct Config
{
	unsigned int windowWidth = 1200;
	unsigned int windowHeight = 800;
	
	bool enableBg = false;

	string bgShaderPath = "UI/shaders/bg_checker.frag";
};

float easeInOutCubic(float x) {
    return x < 0.5 ? 4 * x * x * x : 1 - std::pow(-2 * x + 2, 3) / 2;
}

//todo за это должен отвечать отдельный класс
string buffer = "3/3";
string answerBuffer = "6";

bool enable_bg = false;

int main()
{
	Config config;

	sf::RenderWindow window(sf::VideoMode({config.windowWidth,config.windowHeight}),"SpriteCalc");
	float width = config.windowWidth;
	float height = config.windowHeight;

	sf::Vector2 windowCenter = sf::Vector2f(width/2 - 200,height/2 - 300);

	Background bg(width,height,config.bgShaderPath);

	CalculatorUI calculator;

	calculator.setPosition(windowCenter);
	calculator.setScale(sf::Vector2f(3.0f, 3.0f));
	//calculator.setRotation(sf::degrees(45.0f));

	sf::Clock clock;
	float totalTime = 0.f;

	while (window.isOpen())
	{
		float deltaTime = clock.restart().asSeconds();
		totalTime += deltaTime;

		sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}

				calculator.HandleEvent(*event, window);
		}
		calculator.Update(deltaTime,mousePos);

		window.clear();
		bg.Draw(window, totalTime);

		window.draw(calculator);

		window.display();
		}
	}

