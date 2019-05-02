#pragma once

#include "defines.hpp"

class IComponent;

class Entity
{
public:
    Entity( World* world );
    virtual ~Entity();

    virtual void update( float delta );

    void attach_component( IComponent* component );

private:
    World* m_world;
    basic::Vector< IComponent* > m_components;
};
