#pragma once

#include "glm/glm.hpp"

class Transform
{
public:
    Transform();

    void set_position( const glm::vec3& pos );

    void set_euler_angles( const glm::vec3& pos );

    void  get_matrix(glm::mat4& out) const;

private:
    glm::vec3 m_pos;
    glm::vec3 m_euler_angles;
};
