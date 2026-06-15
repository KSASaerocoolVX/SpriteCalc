module;

#include <SFML/Graphics.hpp>
#include <string>

export module TextNode;

import IMathNode;

export class TextNode : public IMathNode
{
private:
    sf::Text m_text;
    MathMetrics m_metrics;
    sf::Vector2f m_localPosition;

public:
    TextNode(const std::string& text, const sf::Font& font, unsigned int charSize = 56);

    MathMetrics Measure() override;
    void Arrange(sf::Vector2f position) override;

    void UpdateTransform(sf::Vector2f parentPosition, sf::Vector2f parentScale) override;
    void Draw(sf::RenderWindow& window) override;
};