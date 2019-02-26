#include "render.hpp"
#include "basic/file.hpp"
#include "basic/vector.hpp"
#include "camera.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "render_object.hpp"
#include "transform.hpp"
#include "window.hpp"

extern "C" {
#define GLEW_STATIC
#include <GL/glew.h>
}

#include <GL/gl.h>

class Render : public IRender
{
public:
	Render()
        :m_mvp_uniform(0)
		,m_stack()
	{}

    ~Render() override;

    bool
    init( int width, int height ) override
    {
        if ( glewInit( ) != GLEW_OK )
        {
            return false;
        }

        const char* gl_version = reinterpret_cast<const char*>(glGetString( GL_VERSION ));

        LOG( "gl version %s", gl_version );

        glViewport( 0, 0, width, height );
        glClearColor( 0.0, 0.0, 0.0, 1.0 );
        glClearDepth( 1.0 );

        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glEnable( GL_DEPTH_TEST );
		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_BACK);
        glDepthFunc( GL_LESS );


        m_stack.push( glm::mat4( 1.f ) );

        return true;
    }

    void
    draw_begin( ) override
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }

    void
    draw_end( IWindow* wnd ) override
    {
        wnd->swap_buffers( );
    }

    void
    push_mvp( const glm::mat4& mat ) override
    {
        m_stack.push( mat );
        glUniformMatrix4fv( m_mvp_uniform, 1, GL_FALSE, glm::value_ptr( mat ) );
    }

    void
    pop_mvp( ) override
    {
        ASSERT( !m_stack.is_empty( ) );

        m_stack.pop( );

        glUniformMatrix4fv( m_mvp_uniform, 1, GL_FALSE, glm::value_ptr( m_stack.back( ) ) );
    }

private:
    GLint m_mvp_uniform;

    basic::Vector< glm::mat4 > m_stack;
};

IRender::~IRender() {}

IRender*
IRender::create( )
{
    return NEW_OBJ(Render);
}

Render::~Render() {}
