module;

#include <SFML/Graphics.hpp> 
#include <iostream>

module InputScreen;

import AssetManager;
import TextNode;

InputScreen::InputScreen(sf::Vector2f position, const std::string& texturePath) : m_sprite(AssetManager::Instance().GetTexture(texturePath))
{
	setPosition(position);
}


void InputScreen::Update(float deltaTime, sf::Vector2f mousePos)
{
	if (m_inputExpression) {
		sf::Vector2f localMousePos = getInverseTransform().transformPoint(mousePos);
		m_inputExpression->Update(deltaTime, localMousePos);
	}
    if (m_outputExpression) {
        sf::Vector2f localMousePos = getInverseTransform().transformPoint(mousePos);
        m_inputExpression->Update(deltaTime, localMousePos);
    }
}

void InputScreen::HandleEvent(const sf::Event& event, const sf::RenderWindow& window)
{
	if (m_inputExpression) {
		m_inputExpression->HandleEvent(event, window);
	}
    if (m_outputExpression) {
        m_outputExpression->HandleEvent(event, window);
    }
}


void InputScreen::SetInput(std::unique_ptr<IMathNode> node) {
    m_inputExpression = std::move(node);
    if (m_inputExpression) {
        MathMetrics m = m_inputExpression->Measure();
        m_inputExpression->Arrange();

        sf::FloatRect bgBounds = m_sprite.getLocalBounds();

        float paddingX = 8.0f;
        float paddingY = 8.0f;
        float maxWidth = bgBounds.size.x - (paddingX * 2.0f);
        float maxHeight = bgBounds.size.y - (paddingY * 2.0f);

        float scale = 1.0f;
        if (m.width > 0 && m.height > 0) {
            float scaleX = maxWidth / m.width;
            float scaleY = maxHeight / m.height;
            scale = std::min({ 1.0f, scaleX, scaleY });
        }

        m_inputExpression->setScale(sf::Vector2f(scale, scale));

        float screenCenterY = bgBounds.size.y / 2.0f;
        float yPos = screenCenterY - (m.baselineY * scale);

        m_inputExpression->setPosition(sf::Vector2f(paddingX, yPos));
    }
}

void InputScreen::SetOutput(std::unique_ptr<IMathNode> node) {
    m_outputExpression = std::move(node);
    if (m_outputExpression) {
        MathMetrics m = m_outputExpression->Measure();
        m_outputExpression->Arrange();

        sf::FloatRect bgBounds = m_sprite.getLocalBounds();
        float x = bgBounds.size.x - m.width - 20.0f;
        float y = bgBounds.size.y - m.height - 20.0f;

        m_outputExpression->setPosition(sf::Vector2f(x, y));
    }
}

void InputScreen::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    target.draw(m_sprite, states);
    if (m_inputExpression) target.draw(*m_inputExpression, states);
    if (m_outputExpression) target.draw(*m_outputExpression, states);
}