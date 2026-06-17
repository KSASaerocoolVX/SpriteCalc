module;

#include <SFML/Graphics.hpp>
#include <string>
#include <optional>
#include <functional>

export module InputScreen;

import UIElement;
import IMathNode;

export class InputScreen : public UIElement
{
private:
    sf::Sprite m_sprite;
    //std::unique_ptr<IMathNode> m_expression;
    IMathNode* m_inputExpression = nullptr;
    IMathNode* m_outputExpression = nullptr;  


public:
    InputScreen(sf::Vector2f position, const std::string& texturePath);

    void SetInput(IMathNode* node);
    void SetOutput(IMathNode* node);

    void Update(float deltaTime, sf::Vector2f mousePos) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

};


