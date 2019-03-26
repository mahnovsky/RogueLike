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
    out = m_final;
}

void PerspectiveCamera::update()
{
    m_view = glm::lookAt(
        m_position,
        m_direction,
        m_up );

    m_final = m_projection * m_view;
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
    m_projection = glm::ortho( 0.f, m_width, m_height, 0.f, m_near, m_far );

    m_view = glm::lookAt(
            m_position,
            glm::vec3(0,0,0),
            glm::vec3(0,-1,0)
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
    out = m_projection;
}


ICamera::~ICamera(){}

glm::vec3 ICamera::convert_to_world_space(ICamera *cam, const glm::vec3 &screen_pos)
{
    glm::mat4 pv;
    cam->get_matrix(pv);
    glm::mat4 inv = glm::inverse(pv);

    return glm::vec4(screen_pos, 1.f) * inv;
}

glm::vec3 ICamera::convert_to_screen_space(ICamera *cam, const glm::vec3 &world_pos)
{
    glm::mat4 pv;
    cam->get_matrix(pv);

    return glm::vec4(world_pos, 1.f) * pv;
}
