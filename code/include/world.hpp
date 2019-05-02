#pragma once

#include "defines.hpp"

class World
{
public:
    World( glm::vec3 volume );
    ~World();

    void update( float delta );

    void add( Entity* ent );

private:
    glm::vec3 m_volume;

    basic::Vector<Entity*> m_entities;
};

