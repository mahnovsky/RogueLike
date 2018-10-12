#include "transform.hpp"

#include "glm/ext.hpp"

Transform::Transform()
    :m_pos()
    ,m_direction()
    ,m_scale(1.0)
{
}

void Transform::set_position( const glm::vec3& pos )
{
    m_pos = pos;
}

glm::vec3 Transform::get_position() const
{
    return m_pos;
}

void Transform::set_direction( const glm::vec3& direction )
{
    m_direction = direction;
}

glm::vec3 Transform::get_direction() const
{
    return m_direction;
}

void Transform::set_scale( const glm::vec3& scale )
{
    m_scale = scale;
}

glm::vec3 Transform::get_scale( ) const
{
    return m_scale;
}

void Transform::get_matrix( glm::mat4& out ) const
{
    glm::mat4 rot(1.0f); 
    glm::mat4 translate = glm::translate( glm::mat4(), m_pos );
    glm::mat4 scale = glm::scale( m_scale );

    out = translate * rot * scale;
}

