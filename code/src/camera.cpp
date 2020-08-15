#include "camera.hpp"
#include "transform.hpp"
#include "frustum.hpp"

#undef near
#undef far

PerspectiveCamera::PerspectiveCamera(
        float fov, float aspect, float near, float far )
    : m_position( )
    , m_direction( )
    , m_up( )
    , m_projection( )
    , m_view( )
{
	float rad_fov = glm::radians(fov);
    m_projection = glm::perspective( rad_fov, aspect, near, far );

	glm::vec3 center;
	
	get_frustum_minimum_bounding_sphere(m_projection[0][0], m_projection[1][1], near, far, center, m_radius);

	m_offset = center.z;
}

PerspectiveCamera::~PerspectiveCamera( )
{
}

void
PerspectiveCamera::init( const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up )
{
    m_position = pos;
    m_direction = dir;
    m_up = up;

    update( );
}

void
PerspectiveCamera::set_position( const glm::vec3& pos )
{
    m_position = pos;

    update( );
}

void
PerspectiveCamera::set_direction( const glm::vec3& dir )
{
    m_direction = dir;

    update( );
}

void
PerspectiveCamera::set_up( const glm::vec3& up )
{
    m_up = up;

    update( );
}

void
PerspectiveCamera::get_matrix( glm::mat4& out ) const
{
    out = m_final;
}

glm::vec3
PerspectiveCamera::get_position( ) const
{
    return m_position;
}

const std::vector<OctreeObject*>& PerspectiveCamera::get_visible_objects(Octree* octree) const
{
	Sphere s;
	glm::vec3 dir = glm::normalize(m_direction - m_position);
	s.pos = m_position + dir * m_offset;
	s.radius = m_radius;

	m_last_visible_objects.clear();

	ViewFrustum fr;
	extract_view_frustum_planes_from_matrix(m_final, fr, true);

	octree->find_objects(s, fr, m_last_visible_objects);
	
	return m_last_visible_objects;
}

glm::vec2 convert_world_to_screen(
	const glm::vec3& pos,
	const glm::mat4& projection,
	const glm::mat4& view,
	const glm::vec2& view_size,
	const glm::vec2& offset
	)
{
	glm::vec4 world_pos = glm::vec4(pos, 1.f);
	glm::vec4 clip_space_pos = projection * (view * world_pos);
	glm::vec3 ndc_space_pos = glm::vec3(clip_space_pos) / clip_space_pos.w;
	glm::vec2 vp_pos = (glm::vec2(ndc_space_pos.x + 1.0, ndc_space_pos.y + 1.0) / 2.f) * view_size + offset;

	return vp_pos;
}

std::vector<OctreeObject*> PerspectiveCamera::select_objects(glm::vec2 view_size, glm::vec2 left_top, glm::vec2 right_bottom)
{
	std::vector<OctreeObject*> result;

	for (auto obj : m_last_visible_objects)
	{
		if (obj->is_exist(ShapeType::Sphere))
		{
			auto sphere = obj->get_sphere();

			glm::vec2 vp_pos = convert_world_to_screen(sphere.pos, m_projection, m_view, view_size, {});
			glm::vec2 vp_right_pos = convert_world_to_screen(sphere.pos + glm::vec3(1.f, 0.f, 0.f) * sphere.radius, 
				m_projection, m_view, view_size, {});

			float radius = glm::length(vp_pos - vp_right_pos);

			auto center = left_top + (right_bottom - left_top) / 2.f;
			auto dir = glm::normalize(center - vp_pos);

			vp_pos = vp_pos + dir * radius;

			if (vp_pos.x > left_top.x && vp_pos.x < right_bottom.x &&
				vp_pos.y < left_top.y && vp_pos.y > right_bottom.y)
			{
				result.push_back(obj);
			}
		}
	}

	return result;
}

void
PerspectiveCamera::update( )
{
    m_view = glm::lookAt( m_position, m_direction, m_up );

    m_final = m_projection * m_view;
}

OrthoCamera::OrthoCamera( float width, float height, float near, float far )
    : m_projection( )
    , m_width( width )
    , m_height( height )
    , m_near( near )
    , m_far( far )
{
}

void
OrthoCamera::init( const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up )
{
    m_position = pos;

    update( );
}

void
OrthoCamera::update( )
{
    m_projection = glm::ortho( 0.f, m_width, m_height, 0.f, m_near, m_far );

    m_view = glm::lookAt( m_position, glm::vec3( 0, 0, 0 ), glm::vec3( 0, -1, 0 ) );
}

void
OrthoCamera::set_position( const glm::vec3& pos )
{
    m_position = pos;

    update( );
}

void
OrthoCamera::set_direction( const glm::vec3& dir )
{
}

void
OrthoCamera::set_up( const glm::vec3& up )
{
}

glm::vec3
OrthoCamera::get_position( ) const
{
    return m_position;
}

void
OrthoCamera::get_matrix( glm::mat4& out ) const
{
    out = m_projection;
}

ICamera::~ICamera( )
{
}

glm::vec3
ICamera::convert_to_world_space( ICamera* cam, const glm::vec3& screen_pos )
{
    glm::mat4 pv;
    cam->get_matrix( pv );
    glm::mat4 inv = glm::inverse( pv );

    return glm::vec4( screen_pos, 1.f ) * inv;
}

glm::vec3
ICamera::convert_to_screen_space( ICamera* cam, const glm::vec3& world_pos )
{
    glm::mat4 pv;
    cam->get_matrix( pv );

    return glm::vec4( world_pos, 1.f ) * pv;
}
