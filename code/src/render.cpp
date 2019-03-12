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

private:
    GLint m_mvp_uniform;
};

IRender::~IRender() {}

IRender*
IRender::create( )
{
    return NEW_OBJ(Render);
}

Render::~Render() {}
