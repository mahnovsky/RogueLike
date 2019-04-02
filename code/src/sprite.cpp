#include "sprite.hpp"

#include "render_common.hpp"
#include "transform.hpp"
#include "texture.hpp"

Sprite::Sprite( )
    : m_color{255, 255, 255, 255}
    , m_size( 1.f, 1.f, 0.f )
    , m_anchor( 0.5f, 0.5f )
    , m_angle(0.f)
    , m_render_node(nullptr)
{
}

Sprite::~Sprite()
{
    if(m_render_node)
    {
        RenderNode::remove_node( m_render_node );
		m_render_node = nullptr;
    }
}

void
Sprite::init( ShaderProgram* shader, Texture* texture )
{
    ASSERT(shader);
    ASSERT(texture);

    VertexBufferT vb;

    QuadGenerator generator( m_size, m_anchor, m_color );
    TextureRect rect;

    if(!m_frame_name.is_empty() && texture->get_rect(m_frame_name.get_cstr(), rect))
    {
        generator.generate( vb, 0, &rect );
    }
    else
    {
        generator.generate( vb, 0 );
    }

    IndexBuffer ib;
    generator.generate( ib, 0 );

    m_render_node = RenderNode::create_node( shader, texture );
    if( m_render_node )
    {
        m_render_node->init_node( &vb, &ib );
    }
}

void
Sprite::draw( ICamera* camera, IRender* render )
{
    if( m_render_node )
    {
        m_render_node->set_camera(camera);

		m_render_node->draw_node( );
    }
}

void
Sprite::set_position( const glm::vec3& pos )
{
    if(m_render_node)
    {
        m_render_node->get_transform()->set_position( pos );
    }
}

glm::vec3
Sprite::get_position( ) const
{
    if(m_render_node)
    {
        return m_render_node->get_transform()->get_position();
    }

    return glm::vec3();
}

void
Sprite::set_color( basic::uint8 r, basic::uint8 g, basic::uint8 b, basic::uint8 a )
{
    m_color = {r, g, b, a};
    m_render_node->set_color(m_color);
}

void Sprite::update_frame()
{
    TextureRect rect;
    const Texture* texture = m_render_node->get_texture();
    if( m_render_node &&
        texture &&
        !m_frame_name.is_empty() &&
        texture->get_rect(m_frame_name.get_cstr(), rect))
    {
        VertexBufferT vb;
        QuadGenerator generator( m_size, m_anchor, m_color );

        generator.generate( vb, 0, &rect );

        m_render_node->update_vertices( &vb );
    }
}

void
Sprite::set_size( float width, float height )
{
    if( !m_render_node )
    {
        return;
    }
    m_size = {width, height, m_size.z};

    VertexBufferT vb;

    QuadGenerator generator( m_size, m_anchor, m_color );
    TextureRect rect;
    const Texture* texture = m_render_node->get_texture();
    if(texture && !m_frame_name.is_empty() && texture->get_rect(m_frame_name.get_cstr(), rect))
    {
        generator.generate( vb, 0, &rect );
    }
    else
    {
        generator.generate( vb, 0 );
    }

    m_render_node->update_vertices( &vb );
}

void
Sprite::set_anchor( float x, float y )
{
    m_anchor = {x, y};

    VertexBufferT vb;
    QuadGenerator generator( m_size, m_anchor, m_color );
    generator.generate( vb, 0 );

    if( m_render_node )
    {
		m_render_node->update_vertices( &vb );
    }
}

void Sprite::set_angle(float angle)
{
    m_angle = angle;

    if( m_render_node )
    {
        m_render_node->get_transform()->set_euler_angles( {0.f, 0.f, m_angle} );
    }
}

float Sprite::get_angle() const
{
    return m_angle;
}

void Sprite::set_frame_name(const char *name)
{
    m_frame_name = name;

    update_frame();
}
