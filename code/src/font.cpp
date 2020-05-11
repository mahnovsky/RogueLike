#include "font.hpp"
#include "GL/glew.h"

#include "render_common.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace
{
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"
}

namespace se
{
Font::Font(GenericObjectManager* manager, const char* file )
    : FileResource(manager, file)
    , m_object_manager( manager )
    , m_shader( nullptr )
    , m_texture( nullptr )
    , m_height( 32.f )
    , m_cdata( basic::mem_alloc( 96 * sizeof( stbtt_bakedchar ) ) )
{
}

Font::~Font( )
{
    basic::mem_free( m_cdata );
    if ( m_texture )
    {
        m_texture->ref();
    }
    if ( m_shader )
    {
        m_shader->ref();
    }
}

bool
Font::load( ResourceStorage* storage )
{
    m_shader = storage->get_resorce< ShaderProgram >( "text" );
    if ( !m_shader )
    {
        LOG( "Failed load text shader" );

        return false;
    }
    m_shader->ref( );

    std::string path = "fonts/";
    path += get_name( );
    auto data = basic::get_file_content( path.c_str( ) );

    if ( !data.empty( ) )
    {
        const int tw = 512;
        const int th = 512;

        basic::Vector< basic::uint8 > bitmap;
        bitmap.resize( tw * th );

        stbtt_BakeFontBitmap( data.data( ),
                              0,
                              m_height,
                              bitmap.get_raw( ),
                              tw,
                              th,
                              32,
                              96,
                              static_cast< stbtt_bakedchar* >( m_cdata ) );

        std::string name = "bitmap_";
        name += get_name( );

        m_texture = NEW_OBJ( Texture, m_object_manager, name.c_str( ) );
        m_texture->init_font( tw, th, std::move( bitmap ) );
        m_texture->ref( );

        return true;
    }

    return false;
}

RenderNode*
Font::create_text_node( )
{
    RenderNode* node = RenderNode::create_node( m_shader, m_texture );

    return node;
}

void
Font::update( const char* text, RenderNode* node, glm::vec2& size )
{
    ASSERT( node != nullptr );

    if ( !text )
    {
        return;
    }

    VertexBufferT vb;
    IndexBuffer ib;

    float x = 0;
    float y = 0;
    basic::uint16 offset = 0;

    while ( *( text + offset ) )
    {
        char ch = *( text + offset );
        if ( !ch )
        {
            break;
        }

        if ( ch >= 32 )
        {
            stbtt_aligned_quad q;
            int w = static_cast< int >( m_texture->get_width( ) );
            int h = static_cast< int >( m_texture->get_height( ) );

            stbtt_GetBakedQuad(
                    static_cast< stbtt_bakedchar* >( m_cdata ), w, h, ch - 32, &x, &y, &q, 1 );

            auto xmin = q.x0;
            auto xmax = q.x1;
            auto ymin = q.y1;
            auto ymax = q.y0;
            float height = ymin - ymax;
            if ( size.y < height )
            {
                size.y = height;
            }
            glm::vec3 p0 = {xmin, ymin, 0};
            glm::vec3 p1 = {xmin, ymax, 0};
            glm::vec3 p2 = {xmax, ymax, 0};
            glm::vec3 p3 = {xmax, ymin, 0};
            glm::vec2 t0 = {q.s0, q.t1};
            glm::vec2 t1 = {q.s0, q.t0};
            glm::vec2 t2 = {q.s1, q.t0};
            glm::vec2 t3 = {q.s1, q.t1};

            /* https://github.com/0xc0dec/demos/blob/master/src/StbTrueType.cpp*/
            vb.push( {p0, t0} );
            vb.push( {p1, t1} );
            vb.push( {p2, t2} );
            vb.push( {p3, t3} );

            ib.push( 0 + offset * 4 );
            ib.push( 1 + offset * 4 );
            ib.push( 2 + offset * 4 );
            ib.push( 0 + offset * 4 );
            ib.push( 2 + offset * 4 );
            ib.push( 3 + offset * 4 );
        }
        ++offset;
    }
    size.x = x;

    node->reset( &vb, &ib );
}

Font* Font::create( GenericObjectManager* manager, const char* file )
{
    return NEW_OBJ( Font, manager, file );
}
}
