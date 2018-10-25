#pragma once

#include "transform.hpp"

class Camera
{
public:
    Camera();
    ~Camera();

    void init_perspective( float fov, float aspect, float near, float far );

    void init_ortho( float width, float height );

    void set_position( const glm::vec3& pos );

    void get_matrix( glm::mat4& out ) const;

private:
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_up;

    glm::mat4 m_projection;
    glm::mat4 m_view;
};

