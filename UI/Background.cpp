#include "Background.h"
#include <iostream>

    Background::Background(unsigned int width, unsigned int height, const std::string& shaderPath): m_width(width), m_height(height)
    {
		m_shape.setSize(sf::Vector2f(static_cast<float>(m_width), static_cast<float>(m_height)));

		if (!m_shader.loadFromFile(shaderPath, sf::Shader::Type::Fragment)) {
			std::cerr << "failed to load shader from: " << shaderPath << std::endl;
		}
    }

	void Background::Draw(sf::RenderWindow& window)
	{
		window.draw(m_shape, &m_shader);
	}

	//если шейдер зависит от времени
	void Background::Draw(sf::RenderWindow& window, float totalTime)
	{
		m_shader.setUniform("iTime", totalTime);
		m_shader.setUniform("iResolution", sf::Vector2f(window.getSize()));

		window.draw(m_shape, &m_shader);
	}

