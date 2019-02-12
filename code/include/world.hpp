#pragma once

#include "transform.hpp"

class World
{
public:
    World( glm::vec3 volume );
    ~World();

private:
    glm::vec3 m_volume;
};

