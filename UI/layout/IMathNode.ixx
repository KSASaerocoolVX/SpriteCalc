module;
#include <SFML/Graphics.hpp>


export module IMathNode;
import ITransformable;

export struct MathMetrics {
    float width;
    float height;
    float baselineY;
};

export class IMathNode : public ITransformable
{
public:
    virtual ~IMathNode() = default;
    virtual MathMetrics Measure() = 0;
    virtual void Arrange(sf::Vector2f position) = 0;
};