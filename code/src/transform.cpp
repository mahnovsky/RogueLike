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
    glm::vec3 const up( 0.f, 1.f, 0.f );

    glm::mat4 translate = glm::translate( glm::mat4( 1.f ), m_pos );
    glm::mat4 rot = glm::lookAt( m_pos, m_pos + m_forward, up );

    return translate * glm::scale( glm::mat4( 1.f ), m_scale );
}
