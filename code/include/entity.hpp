#pragma once

#include "defines.hpp"

class Entity
{
public:
    Entity( );
    virtual ~Entity( );

    void init( World* world );

private:
    World* m_world;
};
