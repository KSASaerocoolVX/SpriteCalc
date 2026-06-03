#pragma once
class IMovable
{
public:
    virtual ~IMovable() {}
    virtual void Move() = 0;
};