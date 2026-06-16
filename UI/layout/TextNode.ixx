module;

#include <SFML/Graphics.hpp>
#include <string>

export module TextNode;

import IMathNode;

export class TextNode : public IMathNode
{
private:
    sf::Text m_text;

public:
    TextNode(const std::string& text, unsigned int charSize = 56);

    MathMetrics Measure() override;
    void Arrange() override;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};