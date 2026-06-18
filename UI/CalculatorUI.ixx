module;

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

export module CalculatorUI;

import UIElement;
import Button;
import InputScreen;
import MathEditor;
import TextNode;
import IMathNode;

export class CalculatorUI : public UIElement
{
private:
    MathEditor m_editor;
    std::unique_ptr<IMathNode> m_lastAnswer;

    sf::Sprite m_sprite;
    std::vector<std::unique_ptr<UIElement>> children;

    std::string m_inputBuffer;
    InputScreen* m_screenRef = nullptr; 

    std::unique_ptr<sf::RenderWindow> m_helpWindow;
    std::string m_helpTextStr;

    void HandleButtonPress(const std::string& label);
    void ShowHelpWindow();

public:
    //CalculatorUI(sf::Vector2f position);

    CalculatorUI();

    void Update(float deltaTime, sf::Vector2f mousePos) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};
