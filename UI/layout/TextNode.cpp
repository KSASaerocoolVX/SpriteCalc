module;
#include <cmath>
#include <SFML/Graphics.hpp> 
#include <string>
#include <cmath>

module TextNode;

TextNode::TextNode(const std::string& text, const sf::Font& font, unsigned int charSize): m_text(font)
{
    m_text.setString(text);
    m_text.setCharacterSize(charSize);
    m_text.setFillColor(sf::Color(75, 105, 47));
}

MathMetrics TextNode::Measure()
{
    sf::FloatRect bounds = m_text.getLocalBounds();

    m_metrics.width = bounds.size.x;
    m_metrics.height = bounds.size.y;
    m_metrics.baselineY = bounds.size.y;

    return m_metrics;
}

void TextNode::Arrange(sf::Vector2f position)
{
    m_localPosition = position;
}

void TextNode::UpdateTransform(sf::Vector2f parentPosition, sf::Vector2f parentScale)
{
    float xPos = std::round(parentPosition.x + (m_localPosition.x * parentScale.x));
    float yPos = std::round(parentPosition.y + (m_localPosition.y * parentScale.y));

    m_text.setPosition(sf::Vector2f(xPos, yPos));

    float scaleX = parentScale.x * 0.25f;
    float scaleY = parentScale.y * 0.25f;
    m_text.setScale(sf::Vector2f(scaleX, scaleY));
}

void TextNode::Draw(sf::RenderWindow& window)
{
    window.draw(m_text);
}