#pragma once

#include "defines.hpp"
#include "object.hpp"

class IComponent;

class Actor : public Object
{
public:
    Actor( World* world );
    virtual ~Actor();

    void attach_component( IComponent* component );

private:
    World* m_world;
    basic::Vector<IComponent*> m_components;
};
