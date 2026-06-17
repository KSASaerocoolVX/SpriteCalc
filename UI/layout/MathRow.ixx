module;
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
export module MathRow;
import IMathNode;

export class MathRow : public IMathNode
{
private:
    std::vector<std::unique_ptr<IMathNode>> m_children;
    float m_padding = 0.0f; //todo настройка

public:
    MathRow() = default;

    void AddChild(std::unique_ptr<IMathNode> node);
    std::unique_ptr<IMathNode> PopLastChild();

    MathMetrics Measure() override;
    void Arrange() override;
    void Update(float deltaTime, sf::Vector2f mousePos) override;
    void HandleEvent(const sf::Event& event, const sf::RenderWindow& window) override;

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};