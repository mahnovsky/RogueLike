#pragma once

#include "transform.hpp"

class Camera
{
public:
    Camera();

    void init_perspective( float distance, float aspect );

private:
    TransformPtr m_transform;
    float m_distance; 
};

