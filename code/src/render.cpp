#include "render.hpp"
#include "basic/file.hpp"
#include "basic/vector.hpp"
#include "camera.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "transform.hpp"
#include "window.hpp"
#include "render_system.hpp"

#include "resource_storage.hpp"

#include "texture.hpp"
#include "shader.hpp"
#include "static_mesh.hpp"

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

typedef void(*DrawFunc)(struct RenderInstance&);

struct RenderInstance
{
	GLuint program;
	GLuint texture;
	GLuint vao;
	GLuint element_count;
	DrawFunc draw_func;
	glm::mat4 mvp{ 1.0f };
	basic::Color color{ 255, 255, 255, 255 };
};

void draw_insices(RenderInstance& instance)
{
	glDrawElements(GL_TRIANGLES, instance.element_count, GL_UNSIGNED_SHORT, nullptr);
}

void draw_vertices(RenderInstance& instance)
{
	glDrawArrays(GL_TRIANGLES, 0, instance.element_count);
}

void draw_dummy(RenderInstance& instance)
{
}

class OpenGLRenderObject : public IRenderObject
{
public:
	OpenGLRenderObject(ResourceStorage* rs, basic::uint32 uid, basic::Vector<RenderInstance>& instances)
		: m_uid(uid)
		, m_instances(instances)
		, m_rs(rs)
		, m_mesh(nullptr)
		, m_program(nullptr)
		, m_texture(nullptr)
	{
	}

	void on_resource_changed(RenderResourceType type, const basic::String& name)
	{
		if (!name.is_empty())
		{
			apply_changes(type, name);
		}
	}

	void on_component_changed(RenderComponent& comp) override
	{
		for (basic::uint32 i = 0; i < enum2num(RenderResourceType::Count); ++i)
		{
			RenderResourceType type = static_cast<RenderResourceType>(i);

			const basic::String& name = comp.get_resource_name(type);

			if (!name.is_empty())
			{
				apply_changes(type, name);
			}
		}
	}

	void update_mvp(const glm::mat4& mvp) override
	{
		m_instances[m_uid].mvp = mvp;
	}

	void apply_changes(RenderResourceType type, const basic::String& name)
	{
		switch (type)
		{
		case RenderResourceType::StaticMesh:
			update_mesh(name);
			break;
		case RenderResourceType::Texture:
			m_texture = m_rs->get_resorce<Texture>(name.get_cstr());
			break;
		case RenderResourceType::ShaderProgram:
			update_program(name);
			break;
		case RenderResourceType::Count:
			break;
		default:
			break;
		}
	}

	void update_mesh(const basic::String& name)
	{
		if (m_mesh)
		{
			m_mesh->release();
		}

		m_mesh = m_rs->get_resorce<StaticMesh>(name.get_cstr());
		if (!m_mesh)
		{
			return;
		}

		RenderInstance& instance = m_instances[m_uid];
		
		if (m_mesh->get_index_count() > 0)
		{
			instance.draw_func = draw_insices;
			instance.element_count = m_mesh->get_index_count();
		}
		else if(m_mesh->get_vertex_count() > 0)
		{
			instance.draw_func = draw_vertices;
			instance.element_count = m_mesh->get_vertex_count();
		}

		m_mesh->retain();

		glBindVertexArray(instance.vao);
		
		glBindBuffer(GL_ARRAY_BUFFER, m_mesh->get_vbo());

		auto fmt_list = m_mesh->get_fmt_list();
		basic::uint32 i = 0;
		for (const auto& fmt : fmt_list)
		{
			glVertexAttribPointer(i,
				static_cast<GLint>(fmt.size),
				fmt.type,
				fmt.is_normalized,
				sizeof(Vertex),
				reinterpret_cast<const void*>(fmt.offset));

			glEnableVertexAttribArray(i);

			++i;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_mesh->get_ibo());
	}

	void update_program(const basic::String& name)
	{
		if (m_program)
		{
			m_program->release();
		}

		m_program = m_rs->get_resorce<ShaderProgram>(name.get_cstr());
		if (!m_program)
		{
			return;
		}
		m_program->retain();

		m_instances[m_uid].program = m_program->get_handle();
	}

private:
	basic::uint32 m_uid;
	basic::Vector<RenderInstance>& m_instances;
	ResourceStorage* m_rs;
	StaticMesh* m_mesh;
	ShaderProgram* m_program;
	Texture* m_texture;
};

class OpenGLRender : public IRender
{
public:
    OpenGLRender( )
        : m_rs(nullptr)
		, m_mvp_uniform( 0 )
    {
    }

    ~OpenGLRender( ) override;

    bool
    init(ResourceStorage* rs, int width, int height ) override
    {
        if ( glewInit( ) != GLEW_OK )
        {
            return false;
        }
		m_rs = rs;
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
		for (auto& instance : m_instances)
		{
			glBindVertexArray(instance.vao);
			glUseProgram(instance.program);

			set_uniform(instance.program, "MVP", instance.mvp);
			set_uniform(instance.program, "Color", instance.color);

			instance.draw_func(instance);
		}

        wnd->swap_buffers( );
    }

    IGpuFactory*
    get_factory( ) override
    {
        return &m_factory;
    }

	IRenderObject* create_object(RenderComponent& comp) override
	{
		const basic::uint32 uid = m_instances.get_size();
		m_instances.push({0, 0, 0, 0, draw_dummy});

		GLuint vao;
		glGenVertexArrays(1, &vao);
		m_instances[uid].vao = vao;

		auto obj = NEW_OBJ(OpenGLRenderObject, m_rs, uid, m_instances);

		obj->on_component_changed(comp);

		m_objects.push(obj);

		return obj;
	}

	void delete_object(IRenderObject* obj) override
	{
		basic::uint32 index = 0;
		OpenGLRenderObject* internal_obj = dynamic_cast<OpenGLRenderObject*>(obj);
		if (m_objects.find_first(index, internal_obj))
		{
			m_objects.swap_remove(index);
			DELETE_OBJ(obj);
		}
	}

private:
	ResourceStorage* m_rs;
    GLint m_mvp_uniform;
    OpenGLGpuFactory m_factory;
	basic::Vector<OpenGLRenderObject*> m_objects;
	basic::Vector<RenderInstance> m_instances;
};

IRender*
IRender::create( )
{
    return NEW_OBJ( OpenGLRender );
}

OpenGLRender::~OpenGLRender( )
{
}
