#include <SFML/Graphics.hpp>
#include <iostream>
#include <>
using namespace std;

//main потом будет перемещем из Ui

struct Config
{
	unsigned int windowWidth = 800;
	unsigned int windowHeight = 800;
	
	bool enableBg = false;
};

float easeInOutCubic(float x) {
    return x < 0.5 ? 4 * x * x * x : 1 - std::pow(-2 * x + 2, 3) / 2;
}

bool enable_bg = false;

int main()
{
	Config config;
	int width = config.windowWidth;
	int height = config.windowHeight;

	sf::RenderWindow window(sf::VideoMode({ width, height }), "SpriteCalc");

	//bg
	Background

	sf::Texture texture;

	if (!texture.loadFromFile("UI/assets/calculator_ostov.png")) {
		std::cerr << "failed to load texture" << std::endl;
		return -1;
	}

	sf::Sprite sprite(texture);
	//sprite.setPosition({ 10.f, 50.f });


	sf::Shader shader;

	cout << "Application started successfully!" << endl;

	if (!sf::Shader::isAvailable())
	{
		return -1;
	}
	else
	{
		cout << "Shader support is available." << endl;
		if (!shader.loadFromFile("UI/shaders/bg.frag", sf::Shader::Type::Fragment))
		{
			cout << "Failed to load shader." << endl;
			return -1;
		}
	}
	sf::Clock clock;
	float totalTime = 0.f;


	while (window.isOpen())
	{
		sf::Time elasped = clock.restart();
		float deltaTime = elasped.asSeconds();


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

		//sf::FloatRect bounds = sprite.getGlobalBounds();

		//sf::Vector2i mousePos = sf::Mouse::getPosition(window);

		//if (bounds.contains(static_cast<sf::Vector2f>(mousePos))) {

		//	totalTime += deltaTime;


		//	auto positionLerped = sprite.getPosition().x + 10.f;

		//	sf::Vector2f upperLeftCorner = { 0,0 };
		//	sf::Vector2 centerOfTheScreen = { static_cast<float>(window.getSize().x) / 2, static_cast<float>(window.getSize().y) / 2 };

		//	sf::Vector2f upperRightCorner = { static_cast<float>(window.getSize().x), 0 };

		//	sf::Vector2f targetPosition = (centerOfTheScreen * easeInOutCubic(abs(sin(totalTime)))) + (upperRightCorner * static_cast<float>(1.0 - easeInOutCubic(abs(sin(totalTime)))));

		//	sprite.setPosition(targetPosition);
		//}


		//shader.setUniform("iTime", totalTime);
		//shader.setUniform("iResolution", sf::Vector2f(window.getSize()));
		////shader.setUniform("texture1", texture);

		//window.clear();
		//window.draw(shape, &shader);
		//window.draw(sprite);
		//window.display();
	}
}

