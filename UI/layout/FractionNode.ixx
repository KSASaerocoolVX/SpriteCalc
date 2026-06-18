module;

#include <SFML/Graphics.hpp>
#include <memory>

export module FractionNode;

import IMathNode;
import MathRow; 

export class FractionNode : public IMathNode
{
private:
    std::unique_ptr<MathRow> m_numerator;
    std::unique_ptr<MathRow> m_denominator;

    sf::RectangleShape m_bar;

    float m_padding = 4.0f;
    float m_barThickness = 1.0f;

public:
    FractionNode(std::unique_ptr<MathRow> top, std::unique_ptr<MathRow> bottom);

    MathMetrics Measure() override;
    void Arrange() override;

    void Update(float deltaTime, sf::Vector2f mousePos) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    std::string ToString() const override;

    std::vector<IMathNode*> GetInteractableRows() override {
        std::vector<IMathNode*> rows;
        if (m_numerator) rows.push_back(m_numerator.get());
        if (m_denominator) rows.push_back(m_denominator.get());
        return rows;
    }

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};