module;

#include <SFML/Graphics.hpp>
#include <string>

export module TextNode;

import IMathNode;

export class TextNode : public IMathNode
{
private:
    sf::Text m_text;
    bool m_isOperator;

public:
    TextNode(const std::string& text, unsigned int charSize = 56, sf::Color color = sf::Color(75, 105, 47), bool isOperator = false);

    void AppendText(const std::string& append);
    bool PopChar(); //для удаления true если пустой

    MathMetrics Measure() override;
    void Arrange() override;
    std::string ToString() const override;

    bool IsOperator() const override { return m_isOperator; }
    bool IsText() const override { return true; }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;


};