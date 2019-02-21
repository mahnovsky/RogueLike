#include "transform.hpp"

#include "glm/ext.hpp"

Transform::Transform( )
    : m_pos( )
    , m_forward( 0.f, 0.f, 1.f )
    , m_scale( 1.0 )
{
}

void
Transform::set_position( const glm::vec3& pos )
{
    m_pos = pos;
}

glm::vec3
Transform::get_position( ) const
{
    return m_pos;
}

void Transform::set_euler_angles(const glm::vec3 &angles)
{
    m_quat = glm::quat( angles );
}

void
Transform::set_forward( const glm::vec3& direction )
{
    m_forward = glm::normalize( direction );
}

glm::vec3
Transform::get_forward( ) const
{
    return m_forward;
}

void
Transform::set_scale( const glm::vec3& scale )
{
    m_scale = scale;
}

glm::vec3
Transform::get_scale( ) const
{
    return m_scale;
}

glm::mat4
Transform::get_matrix( ) const
{
    glm::mat4 const identity( 1.f );

    glm::mat4 translate = glm::translate( identity, m_pos );
    glm::mat4 rot(1.f);// = glm::mat4_cast( m_quat );
    glm::mat4 scale = glm::scale( identity, m_scale );

    return translate * rot * scale;
}
