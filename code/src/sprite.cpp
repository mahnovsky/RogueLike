#include "sprite.hpp"

Sprite::Sprite( )
    : m_object( )
    , m_color{255, 255, 255, 255}
    , m_texture( nullptr )
	, m_shader( nullptr )
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
        remove_node( m_render_node );
    }
}

void
Sprite::init( ShaderProgram* shader, Texture* texture )
{
    VertexBuffer vb;

    QuadGenerator generator( m_size, m_anchor, m_color );
    generator.generate( vb, 0 );

    //m_object.set_vertex_buffer( std::move( vb ) );

    IndexBuffer ib;
    generator.generate( ib, 0 );

    //m_object.set_index_buffer( std::move( ib ) );

    /*m_object.init( );

    m_object.set_texture( texture );
    m_object.set_shader( shader );
    m_object.get_transform( )->set_forward( {0.f, 0.f, 1.f} );*/

    m_render_node = create_node( shader, texture );
    if( m_render_node )
    {
        init_node( m_render_node, &vb, &ib );
    }
}

void
Sprite::draw( ICamera* camera, IRender* render )
{
    //m_object.draw( render, camera );

    if( m_render_node )
    {
        m_render_node->camera = camera;

        draw_node( m_render_node );
    }
}

void
Sprite::set_position( const glm::vec3& pos )
{
    //m_object.get_transform( )->set_position( pos );
    if(m_render_node)
    {
        m_render_node->transform->set_position( pos );
    }
}

glm::vec3
Sprite::get_position( ) const
{
    if(m_render_node)
    {
        return m_render_node->transform->get_position();
    }

    return glm::vec3(); //m_object.get_transform( )->get_position( );
}

void
Sprite::set_color( basic::uint8 r, basic::uint8 g, basic::uint8 b, basic::uint8 a )
{
    m_color = {r, g, b, a};

    if ( m_object.is_initialized( ) )
    {
        m_object.update( &Sprite::update_color, this );
    }
}

void
Sprite::set_size( float width, float height )
{
    m_size = {width, height, m_size.z};

    VertexBuffer vb;

    QuadGenerator generator( m_size, m_anchor, m_color );
    generator.generate( vb, 0 );

    if( m_render_node && m_render_node->vertex_object != 0 )
    {
        update_vertices( m_render_node, &vb );
    }
}

void
Sprite::set_anchor( float x, float y )
{
    m_anchor = {x, y};

    VertexBuffer vb;
    QuadGenerator generator( m_size, m_anchor, m_color );
    generator.generate( vb, 0 );

    if( m_render_node && m_render_node->vertex_object != 0 )
    {
        update_vertices( m_render_node, &vb );
    }
}

void Sprite::set_angle(float angle)
{
    m_angle = angle;

    if( m_render_node )
    {
        m_render_node->transform->set_euler_angles( {0.f, 0.f, m_angle} );
    }
}

float Sprite::get_angle() const
{
    return m_angle;
}

void
Sprite::update_color( Vertex* v, void* user_data )
{
    Sprite* sp = static_cast< Sprite* >( user_data );

    v->color = sp->m_color;
}

void
Sprite::update_size( Vertex* /*v*/, void* /*user_data*/ )
{
    //Sprite* sp = static_cast< Sprite* >( user_data );
}
