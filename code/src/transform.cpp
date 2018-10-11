#include "transform.hpp"


Transform::Transform()
    :m_pos()
    ,m_euler_angles()
{
}

void Transform::set_position( const glm::vec3& pos )
{
    m_pos = pos;
}

void Transform::set_euler_angles( const glm::vec3& angles )
{
    m_euler_angles = angles;
}

void  get_matrix(glm::mat4& out) const
{

}

