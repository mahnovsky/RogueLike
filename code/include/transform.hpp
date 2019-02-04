#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

class Transform
{
public:
    Transform();

    void set_position( const glm::vec3& pos );

    glm::vec3 get_position() const;

    void set_euler_angles( const glm::vec3& angles );

    void set_forward( const glm::vec3& dir );

    glm::vec3 get_forward() const;

    void set_scale( const glm::vec3& scale );

    glm::vec3 get_scale() const;

	glm::mat4 get_matrix() const;

private:
    glm::vec3 m_pos;
    glm::quat m_quat;
    glm::vec3 m_forward;
	glm::vec3 m_up;
    glm::vec3 m_scale;
};
