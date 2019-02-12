#pragma once

#include "transform.hpp"

class World;

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
