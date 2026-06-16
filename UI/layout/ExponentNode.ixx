module;
#include <SFML/Graphics.hpp>
#include <memory>
export module ExponentNode;
import IMathNode;

export class ExponentNode : public IMathNode
{
private:
    std::unique_ptr<IMathNode> m_base;
    std::unique_ptr<IMathNode> m_power;

public:
    ExponentNode(std::unique_ptr<IMathNode> base, std::unique_ptr<IMathNode> power);
    MathMetrics Measure() override;
    void Arrange() override;
protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};