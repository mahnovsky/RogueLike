#include "line.hpp"

#include "shader.hpp"
#include "render.hpp"
#include "camera.hpp"

Line::Line()
    : m_width(2.f)
    , m_pos0()
    , m_pos1(2.0, 2.0, 0.f)
    , m_color{ 255, 255, 255, 255 }
    , m_render_node(nullptr)
{

}

Line::~Line()
{
    remove_node( m_render_node );
}

void Line::set_color(const basic::Color &color)
{
    m_color = color;

    m_render_node->color = color;
}

void Line::set_pos0(const glm::vec3 &p0)
{
    m_pos0 = p0;

    set_coords( m_pos0, m_pos1 );
}

void Line::set_pos1(const glm::vec3 &p1)
{
    m_pos1 = p1;

    set_coords( m_pos0, m_pos1 );
}

void Line::set_coords(const glm::vec3 & p0, const glm::vec3 & p1)
{
    m_pos0 = p0;
    m_pos1 = p1;

    VertexBufferT buff;
    update( buff );

    update_vertices( m_render_node, &buff );
}

void Line::set_width(float width)
{
    m_width = width;
    VertexBufferT buff;
    update( buff );

    update_vertices( m_render_node, &buff );
}

void Line::init(ShaderProgram* shader)
{
    ASSERT(shader != nullptr);

    m_render_node = create_node( shader, nullptr );

    if( !m_render_node )
    {
        return;
    }

    VertexBufferT vertices;
    IndexBuffer indices;
    basic::uint16 raw_indices[] { 0, 1, 2, 3, 2, 1 };
    indices.init( raw_indices, 6 );

    m_render_node->color = m_color;

    update( vertices );

    init_node( m_render_node, &vertices, &indices );
}

void Line::draw( IRender* render, ICamera* cam ) const
{
    if( m_render_node->camera != cam )
    {
        m_render_node->camera = cam;
    }

    draw_node( m_render_node );
}

basic::uint32 Line::get_element_count() const
{
    return basic::uint32();
}

void Line::get_matrix(glm::mat4 & out) const
{
    out = glm::mat4();
}

void Line::update( VertexBufferT& vb )
{
    glm::vec3 delta = m_pos1 - m_pos0;
    float sq_dist = glm::dot(delta, delta);
    ASSERT(sq_dist > 0.1f);
    ASSERT(m_width > 0.1f);

    glm::vec3 up{ 0.f, 0.f, 1.f };
    glm::vec3 dir = glm::normalize(glm::cross(delta, up));

    const float half_width = m_width / 2;
    glm::vec3 p1 = (dir * half_width) + m_pos0;
    glm::vec3 p2 = (dir * half_width) + m_pos1;
    glm::vec3 p3 = (dir * -half_width) + m_pos0;
    glm::vec3 p4 = (dir * -half_width) + m_pos1;

    vb.push({ p1, glm::vec2() });
    vb.push({ p2, glm::vec2() });
    vb.push({ p3, glm::vec2() });
    vb.push({ p4, glm::vec2() });
}
