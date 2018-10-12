#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Transform
{
public:
    Transform();

    void set_position( const glm::vec3& pos );

    glm::vec3 get_position() const;

    void set_direction( const glm::vec3& dir );

    glm::vec3 get_direction() const;

    void set_scale( const glm::vec3& scale );

    glm::vec3 get_scale() const;

    void get_matrix(glm::mat4& out) const;

private:
    glm::vec3 m_pos;
    glm::vec3 m_direction;
    glm::vec3 m_scale;
};
