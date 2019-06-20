#include "transform.hpp"

#include "glm/ext.hpp"

Transform::Transform( )
    : m_parent( nullptr )
    , m_final_mat( 1.f )
    , m_pos( )
    , m_pivot_point( )
    , m_quat( )
    , m_scale( 1.f )
{
}

void
Transform::set_parent( Transform* parent )
{
    ASSERT( parent );
    if ( m_parent )
    {
        m_parent->remove( this );
    }
    m_parent = parent;

    update_final_matrix( );
}

void
Transform::add_child( Transform* child )
{
    m_children.push( child );

    child->set_parent( this );
}

void
Transform::remove( Transform* child )
{
    m_children.remove_by_value( child );
}

void
Transform::set_position( const glm::vec3& pos )
{
    m_pos = pos;

    update_final_matrix( );
}

glm::vec3
Transform::get_position( ) const
{
    return m_pos;
}

void
Transform::set_pivot_point( const glm::vec3& pivot )
{
    m_pivot_point = pivot;

    update_final_matrix( );
}

glm::vec3
Transform::get_pivot_point( ) const
{
    return m_pivot_point;
}

void
Transform::set_euler_angles( const glm::vec3& angles )
{
    m_quat = glm::quat( angles );

    update_final_matrix( );
}

void
Transform::set_scale( const glm::vec3& scale )
{
    m_scale = scale;

    update_final_matrix( );
}

glm::vec3
Transform::get_scale( ) const
{
    return m_scale;
}

glm::mat4
Transform::get_matrix( ) const
{
    return m_final_mat;
}

void
Transform::update_final_matrix( ) const
{
    glm::mat4 const identity( 1.f );

    glm::mat4 pivot = glm::translate( identity, -m_pivot_point );
    glm::mat4 translate = glm::translate( identity, m_pos );
    glm::mat4 rot = glm::mat4_cast( m_quat );
    glm::mat4 scale = glm::scale( identity, m_scale );

    m_final_mat = translate * rot * pivot * scale;
    if ( m_parent )
    {
        m_final_mat = m_final_mat * m_parent->get_matrix( );
    }

    for ( Transform* child : m_children )
    {
        child->update_final_matrix( );
    }
}
