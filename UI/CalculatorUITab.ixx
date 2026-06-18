module;
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>

export module CalculatorUITab;

import UIElement;
import Button;

export class CalculatorUITab : public UIElement
{
private:
    sf::Sprite m_sprite; 
    sf::Text m_handleText;

    std::vector<std::unique_ptr<Button>> m_buttons;

    bool m_isOpen = false;
    float m_slideProgress = 0.0f;

    float m_panelWidth;
    float m_handleWidth;
    float m_panelHeight;
    sf::Vector2f m_basePosition;

public:
    CalculatorUITab(sf::Vector2f basePosition, const std::string& texturePath, float handleWidth, const std::string& label);

    void AddButton(std::unique_ptr<Button> button);

    void Update(float deltaTime, sf::Vector2f mousePos) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};