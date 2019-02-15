#pragma once

#include "defines.hpp"

class IComponent;

class Actor
{
public:
    Actor(World* world);
    virtual ~Actor();

    void attach_component( IComponent* component );

private:
    World* m_world;
    basic::Vector<IComponent*> m_components;
};
