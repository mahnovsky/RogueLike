#include "camera.hpp"
#include "transform.hpp"

PerspectiveCamera::PerspectiveCamera(ObjectManager *manager, float fov, float aspect, float near, float far )
    : Object( manager )
    , m_position()
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

OrthoCamera::OrthoCamera(ObjectManager *manager, float width, float height, float near, float far )
    :Object (manager)
    ,m_projection()
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

    m_projection = glm::ortho( 0.f, m_width, m_height, 0.f, m_near, m_far );

    m_view = glm::lookAt(

            m_position, // Camera is at (0,0,5), in World Space

            glm::vec3(0,0,0), // and looks at the origin

            glm::vec3(0,-1,0)  // Head is up (set to 0,-1,0 to look upside-down)

        );
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
    out = m_projection ;
}


ICamera::~ICamera(){}
