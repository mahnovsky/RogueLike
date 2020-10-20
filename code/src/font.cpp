#include "font.hpp"
#include "GL/glew.h"

#include "render_common.hpp"
#include "opengl/resource_shader.hpp"
#include "texture.hpp"
#include "render.hpp"

namespace
{
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"
}

namespace se
{
	using namespace std::string_literals;

Font::Font( const char* file )
    : FileResource(file)
    , m_shader( nullptr )
    , m_texture( nullptr )
    , m_height( 32.f )
    , m_cdata( basic::mem_alloc( 96 * sizeof( stbtt_bakedchar ) ) )
{
}

Font::~Font( )
{
    basic::mem_free( m_cdata );
}

bool Font::load( core::ResourceStorage* storage )
{
    m_shader = storage->get_resorce< ogl::ShaderProgram >( "text" );
    if ( !m_shader )
    {
        LOG( "Failed load text shader" );

        return false;
    }

    std::string path = "fonts/"s + get_file_name().data();
    
    auto data = basic::get_file_content( path.c_str( ) );

    if ( !data.empty( ) )
    {
        const int tw = 512;
        const int th = 512;

        std::vector< uint8_t > bitmap;
        bitmap.resize( tw * th );

        stbtt_BakeFontBitmap( data.data( ), 0, m_height, bitmap.data( ), tw, th, 32, 96,
                              static_cast< stbtt_bakedchar* >( m_cdata ) );

		m_texture_name = "font_bitmap_"s + get_file_name().data();

		m_texture = std::make_shared<Texture>(m_texture_name.c_str());
        //m_texture = std::dynamic_pointer_cast<Texture>( texture->shared_from_this() );
        m_texture->init_font( tw, th, std::move( bitmap ) );
		storage->add_resource(m_texture);

        return true;
    }

    return false;
}

void Font::update( const char* text, IRenderObject* text_object, glm::vec2& size )
{
    if ( !text || !text_object )
    {
        return;
    }

    VertexBufferT vb;
    IndexBuffer ib;

    float x = 0;
    float y = 0;
    uint16_t offset = 0;
	auto& mesh_data = text_object->get_mesh_data();
	mesh_data.indices.clear();

	text_object->set_resource(RenderResourceType::ShaderProgram, "text");
	text_object->set_resource(RenderResourceType::Texture, m_texture_name);
	text_object->set_vertex_buffer_usage(VertexBufferUsage::Dynamic);

    while ( *( text + offset ) )
    {
        char ch = *( text + offset );
        if ( !ch )
        {
            break;
        }

        if ( ch >= 32 )
        {
            ::stbtt_aligned_quad q;
            int w = static_cast< int >( m_texture->get_width( ) );
            int h = static_cast< int >( m_texture->get_height( ) );

            stbtt_GetBakedQuad(
                    static_cast< ::stbtt_bakedchar* >( m_cdata ), w, h, ch - 32, &x, &y, &q, 1 );

            auto xmin = q.x0;
            auto xmax = q.x1;
            auto ymin = -q.y1;
            auto ymax = -q.y0;
            float height = fabs(ymin - ymax);
            if ( size.y < height )
            {
                size.y = height;
            }
            glm::vec3 p0 = {xmin, ymin, 0};
            glm::vec3 p1 = {xmin, ymax, 0};
            glm::vec3 p2 = {xmax, ymax, 0};
            glm::vec3 p3 = {xmax, ymin, 0};
			/*
			info.uvs[0] = { quad.s0, quad.t1 };
			info.uvs[1] = { quad.s0, quad.t0 };
			info.uvs[2] = { quad.s1, quad.t0 };
			info.uvs[3] = { quad.s1, quad.t1 };*/

            glm::vec2 t0 = {q.s0, q.t1};
            glm::vec2 t1 = {q.s0, q.t0};
            glm::vec2 t2 = {q.s1, q.t0};
            glm::vec2 t3 = {q.s1, q.t1};

            /* https://github.com/0xc0dec/demos/blob/master/src/StbTrueType.cpp*/
            vb.push_back( {p0, t0} );
            vb.push_back( {p1, t1} );
            vb.push_back( {p2, t2} );
            vb.push_back( {p3, t3} );

			mesh_data.indices.push_back( 0 + offset * 4 );
			mesh_data.indices.push_back( 1 + offset * 4 );
			mesh_data.indices.push_back( 2 + offset * 4 );
			mesh_data.indices.push_back( 0 + offset * 4 );
			mesh_data.indices.push_back( 2 + offset * 4 );
			mesh_data.indices.push_back( 3 + offset * 4 );
        }
        ++offset;
    }
    size.x = x;
	
	setup_vertices(mesh_data.vertex_data, std::move(vb));
	
	text_object->update_mesh_data();
}

Font* Font::create( const char* file )
{
    return NEW_OBJ( Font, file );
}
}
