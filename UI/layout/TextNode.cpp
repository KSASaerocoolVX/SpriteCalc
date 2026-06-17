module;
#include <cmath>
#include <SFML/Graphics.hpp> 
#include <string>
#include <cmath>

module TextNode;
import AssetManager;

//

TextNode::TextNode(const std::string& text, unsigned int charSize, sf::Color color, bool isOperator) : m_text(AssetManager::Instance().GetFont("UI/assets/RetroGaming.ttf")), m_isOperator(isOperator)
{
    m_text.setString(text);
    m_text.setCharacterSize(charSize);
    m_text.setFillColor(color);

    m_text.setScale(sf::Vector2f(0.25f, 0.25f));
}

void TextNode::AppendText(const std::string& append)
{
    m_text.setString(m_text.getString() + append);
}

bool TextNode::PopChar()
{
    std::string str = m_text.getString();
    if (!str.empty() && !m_isOperator) {
        str.pop_back();
        m_text.setString(str);
    }
    return str.empty();
}

MathMetrics TextNode::Measure()
{
    sf::FloatRect bounds = m_text.getLocalBounds();
    m_metrics.width = (bounds.position.x + bounds.size.x) * 0.25f;

    float fixedHeight = m_text.getCharacterSize() * 0.25f;
    m_metrics.height = fixedHeight;

    m_metrics.baselineY = fixedHeight * 0.5f;

    return m_metrics;
}

void TextNode::Arrange()
{
}



std::string TextNode::ToString() const
{
    return m_text.getString();
}

void TextNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_text, states);
}
