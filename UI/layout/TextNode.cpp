module;
#include <cmath>
#include <SFML/Graphics.hpp> 
#include <string>
#include <cmath>

module TextNode;
import AssetManager;

//

TextNode::TextNode(const std::string& text, unsigned int charSize) : m_text(AssetManager::Instance().GetFont("UI/assets/RetroGaming.ttf"))
{
    m_text.setString(text);
    m_text.setCharacterSize(charSize);
    m_text.setFillColor(sf::Color(75, 105, 47));

    m_text.setScale(sf::Vector2f(0.25f, 0.25f));
}

MathMetrics TextNode::Measure()
{
    sf::FloatRect bounds = m_text.getLocalBounds();

    m_metrics.width = bounds.size.x * 0.25f;
    m_metrics.height = bounds.size.y * 0.25f;
    m_metrics.baselineY = m_metrics.height;

    return m_metrics;
}

void TextNode::Arrange()
{
}

void TextNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_text, states);
}
