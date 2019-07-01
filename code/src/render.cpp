#include "render.hpp"
#include "basic/file.hpp"
#include "basic/vector.hpp"
#include "camera.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "transform.hpp"
#include "window.hpp"

extern "C" {
#define GLEW_STATIC
#include <GL/glew.h>
}

#include <GL/gl.h>

class OpenGLVertexBuffer : public IVertexBuffer
{
public:
    OpenGLVertexBuffer( )
        : m_count( 0 )
        , m_handle( 0 )
    {
    }

    ~OpenGLVertexBuffer( ) override
    {
        glDeleteBuffers( 1, &m_handle );
    }

    basic::uint32
    count( ) const override
    {
        return m_count;
    }

    void
    bind( bool on ) override
    {
        glBindBuffer( GL_ARRAY_BUFFER, on ? m_handle : 0 );
    }

    void
    load( VertexBuffer buffer ) override
    {
        m_count = size( buffer );

        glGenBuffers( 1, &m_handle );
        glBindBuffer( GL_ARRAY_BUFFER, m_handle );
        glBufferData(
                GL_ARRAY_BUFFER, m_count * sizeof( Vertex ), buffer.get_raw( ), GL_STATIC_DRAW );

        m_fmt_list = get_fmt_list( buffer.get_raw( ) );
    }

    void
    load( VertexBufferP buffer ) override
    {
        m_count = size( buffer );

        glGenBuffers( 1, &m_handle );
        glBindBuffer( GL_ARRAY_BUFFER, m_handle );
        glBufferData( GL_ARRAY_BUFFER,
                      m_count * sizeof( VertexBufferP ),
                      buffer.get_raw( ),
                      GL_STATIC_DRAW );

        m_fmt_list = get_fmt_list( buffer.get_raw( ) );
    }

    void
    apply_fmt( ) const override
    {
        basic::uint32 i = 0;
        for ( const auto& fmt : m_fmt_list )
        {
            glVertexAttribPointer( i,
                                   static_cast< GLint >( fmt.size ),
                                   fmt.type,
                                   fmt.is_normalized,
                                   sizeof( Vertex ),
                                   reinterpret_cast< const void* >( fmt.offset ) );

            glEnableVertexAttribArray( i );

            ++i;
        }
    }

private:
    basic::uint32 m_count;
    GLuint m_handle;
    basic::Vector< VertexFMT > m_fmt_list;
};

class OpenGLIndexBuffer : public IIndexBuffer
{
public:
    OpenGLIndexBuffer( )
        : m_count( 0 )
        , m_handle( 0 )
    {
    }

    ~OpenGLIndexBuffer( ) override
    {
        glDeleteBuffers( 1, &m_handle );
    }

    basic::uint32
    count( ) const override
    {
        return m_count;
    }

    void
    bind( bool on ) override
    {
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, on ? m_handle : 0 );
    }

    void
    load( IndexBuffer buffer ) override
    {
        m_count = size( buffer );

        glGenBuffers( 1, &m_handle );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_handle );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                      m_count * sizeof( basic::uint16 ),
                      buffer.get_raw( ),
                      GL_STATIC_DRAW );
    }

private:
    basic::uint32 m_count;
    GLuint m_handle;
};

class OpenGLGpuLayer : public IGpuObjectLayer
{
public:
    OpenGLGpuLayer( )
        : m_handle( 0 )
    {
        glGenVertexArrays( 1, &m_handle );
    }

    ~OpenGLGpuLayer( ) override
    {
        glDeleteVertexArrays( 1, &m_handle );
    }

    void
    bind( bool on ) override
    {
        glBindVertexArray( on ? m_handle : 0 );
    }

    void
    attach_object( IGpuObject* obj ) override
    {
        bind( true );

        obj->bind( true );

        bind( false );
    }

private:
    GLuint m_handle;
};

class OpenGLGpuFactory : public IGpuFactory
{
public:
    IVertexBuffer*
    create_vertex_buffer( ) const
    {
        return NEW_OBJ( OpenGLVertexBuffer );
    }

    IIndexBuffer*
    create_index_buffer( ) const
    {
        return NEW_OBJ( OpenGLIndexBuffer );
    }

    IGpuObjectLayer*
    create_layer( ) const
    {
        return NEW_OBJ( OpenGLGpuLayer );
    }
};

class OpenGLRender : public IRender
{
public:
    OpenGLRender( )
        : m_mvp_uniform( 0 )
    {
    }

    ~OpenGLRender( ) override;

    bool
    init( int width, int height ) override
    {
        if ( glewInit( ) != GLEW_OK )
        {
            return false;
        }

        const char* gl_version = reinterpret_cast< const char* >( glGetString( GL_VERSION ) );

        LOG( "gl version %s", gl_version );

        glViewport( 0, 0, width, height );
        glClearColor( 0.0, 0.0, 0.0, 1.0 );
        glClearDepth( 1.0 );

        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glEnable( GL_DEPTH_TEST );
        // glEnable( GL_CULL_FACE );
        // glCullFace(GL_BACK);
        glDepthFunc( GL_LESS );
        // glDepthMask( GL_TRUE );

        return true;
    }

    void
    clear( ) override
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }

    void
    present( IWindow* wnd ) override
    {
        wnd->swap_buffers( );
    }

    IGpuFactory*
    get_factory( ) override
    {
        return &m_factory;
    }

private:
    GLint m_mvp_uniform;
    OpenGLGpuFactory m_factory;
};

IRender::~IRender( )
{
}

IRender*
IRender::create( )
{
    return NEW_OBJ( OpenGLRender );
}

OpenGLRender::~OpenGLRender( )
{
}
