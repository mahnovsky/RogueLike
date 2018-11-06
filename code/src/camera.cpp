#include "camera.hpp"
#include "glm/glm.hpp"

PerspectiveCamera::PerspectiveCamera( float fov, float aspect, float near, float far )
    : m_position()
    , m_direction()
    , m_up()
    , m_projection()
    , m_view()
{
    m_projection = glm::perspective( glm::radians(fov), aspect, near, far );
}

PerspectiveCamera::~PerspectiveCamera()
{
}

void PerspectiveCamera::init( 
                const glm::vec3& pos, 
                const glm::vec3& dir, 
                const glm::vec3& up )
{
    m_position = pos;
    m_direction = dir;
    m_up = up;

    update();
} 

void PerspectiveCamera::set_position( const glm::vec3& pos )
{
    m_position = pos;

    update();
}

void PerspectiveCamera::set_direction( const glm::vec3& dir )
{
    m_direction = dir;

    update();
}

void PerspectiveCamera::set_up( const glm::vec3& up )
{
    m_up = up;

    update();
}

void PerspectiveCamera::get_matrix( glm::mat4& out ) const
{
    out = m_projection * m_view;
}

void PerspectiveCamera::update()
{
    m_view = glm::lookAt(
        m_position,
        m_direction,
        m_up );
}

OrthoCamera::OrthoCamera( float width, float height, float near, float far )
    :m_projection()
    ,m_width( width )
    ,m_height( height )
    ,m_near( near )
    ,m_far( far )
{
 
}

void OrthoCamera::init(
                const glm::vec3& pos, 
                const glm::vec3& dir, 
                const glm::vec3& up ) 
{
    m_position = pos;

    update();
}

void OrthoCamera::update()
{
    glm::vec3 pos = m_position;
    const float left = pos.x - m_width / 2;
    const float right = pos.x + m_width / 2;
    const float top = pos.y + m_height / 2;
    const float bottom = pos.y - m_height / 2;

    m_projection = glm::ortho( left, right, bottom, top, m_near, m_far ); 
}

void OrthoCamera::set_position( const glm::vec3& pos )
{
    m_position = pos;

    update();
}

void OrthoCamera::set_direction( const glm::vec3& dir )
{
}

void OrthoCamera::set_up( const glm::vec3& up )
{
}

void OrthoCamera::get_matrix( glm::mat4& out ) const
{
    out = m_projection;
}

