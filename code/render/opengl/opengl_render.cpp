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
#include "opengl/resource_shader.hpp"
#include "static_mesh.hpp"

#include "opengl_wrapper.hpp"

using namespace ogl;

extern "C" {
#define GLEW_STATIC
#include <GL/glew.h>
}

#include <GL/gl.h>

GLenum g_OpenGLError = GL_NO_ERROR;

struct RenderObjectData
{
	ogl::VertexDrawMode draw_mode = ogl::VertexDrawMode::Triangles;
	GLuint vao;
	GLuint texture;
	GLuint program;
	GLuint vertex_count;
	GLuint index_count;
	basic::Color color;
	glm::mat4 mvp;
	uint32_t flags;
};

class OpenGLRenderObject final : public IRenderObject
{
public:
	OpenGLRenderObject() = delete;

	OpenGLRenderObject(ResourceStorage* rs)
		: m_rs(rs)
		, m_mesh(nullptr)
		, m_program(nullptr)
		, m_texture(nullptr)
		, m_instance()
	{
		memset(&m_instance, 0, sizeof(RenderObjectData));
		m_instance.draw_mode = ogl::VertexDrawMode::Triangles;
		m_instance.flags = 0;
	}

	~OpenGLRenderObject() override
	{
		if (m_instance.vao != INVALID_HANDLE)
		{
			ogl::delete_vertex_array(m_instance.vao);
		}
	}

	void set_resource(RenderResourceType type, const std::string& name) override
	{
		if (!name.empty())
		{
			apply_changes(type, name);
		}
	}

	void update_mvp(const glm::mat4& mvp) override
	{
		m_instance.mvp = mvp;
	}

	void apply_changes(RenderResourceType type, const std::string& name)
	{
		switch (type)
		{
		case RenderResourceType::StaticMesh:
			update_mesh(name);
			break;
		case RenderResourceType::Texture:
			update_texture(name);
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

	void update_mesh(const std::string& name)
	{
		m_mesh = m_rs->get_resorce<StaticMesh>(name.c_str());
		if (m_mesh)
		{
			if (m_instance.vao != INVALID_HANDLE)
			{
				ogl::delete_vertex_array(m_instance.vao);
				m_instance.vao = INVALID_HANDLE;
			}

			if (auto res = ogl::create_vertex_array())
			{
				m_instance.vao = res.value();
			}

			setup_buffers();
		}
	}

	void update_program(const std::string& name)
	{
		m_program = m_rs->get_resorce<ogl::ShaderProgram>(name.c_str());
		if (m_program)
		{
			m_instance.program = m_program->get_handle();
		}
	}

	void update_color(basic::Color color) override
	{
		/*GLuint ucolor = 
			((GLuint)color.red << 24) |
			((GLuint)color.green << 16) |
			((GLuint)color.blue << 8) |
			((GLuint)color.alpha);*/

		m_instance.color = color;
	}

	void update_texture(const std::string& name)
	{
		m_texture = m_rs->get_resorce<Texture>(name.c_str());
		if (m_texture)
		{
			m_instance.texture = m_texture->get_handle();
		}
	}
	/*
	void draw_object() const
	{
		CHECKED_CALL(glBindVertexArray, m_vao);
		CHECKED_CALL(glUseProgram, m_program->get_handle());

		if (m_texture)
		{
			CHECKED_CALL(glActiveTexture, GL_TEXTURE0);

			CHECKED_CALL(glBindTexture, GL_TEXTURE_2D, m_texture->get_handle());
		}

		auto program = m_program->get_handle();
		set_uniform(program, "MVP", m_mvp);
		set_uniform(program, "Color", m_color);

		if (m_mesh->get_index_count() > 0)
		{
			CHECKED_CALL(glDrawElements, GL_TRIANGLES, m_mesh->get_index_count() * sizeof(short), GL_UNSIGNED_SHORT, nullptr);
		}
		else if (m_mesh->get_vertex_count() > 0)
		{
			CHECKED_CALL(glDrawArrays, GL_TRIANGLES, 0, m_mesh->get_vertex_count());
		}
	}*/

	void update_mesh_data(const MeshData& data)
	{
		if (m_instance.vao == INVALID_HANDLE)
		{
			auto res = ogl::create_vertex_array();

			m_instance.vao = res.value_or(INVALID_HANDLE);
		}

		if (m_instance.vao != INVALID_HANDLE)
		{
			m_mesh = std::make_shared<StaticMesh>("mesh");

			if (m_mesh->init(data, m_buffer_usage))
			{
				setup_buffers();
			}
		}
	}

	void setup_buffers()
	{
		m_instance.index_count = m_mesh->get_index_count();
		m_instance.vertex_count = m_mesh->get_vertex_count();

		ogl::bind_vertex_array(m_instance.vao);
		ogl::bind_buffer(m_mesh->get_vbo(), ogl::BufferType::Array);

		auto fmt_list = m_mesh->get_fmt_list();
		basic::uint32 i = 0;
		const uint32_t element_size = m_mesh->get_element_size();
		
		for (const auto& fmt : fmt_list)
		{
			ogl::vertex_attrib_pointer(i,
				fmt.size,
				fmt.type,
				fmt.is_normalized,
				element_size,
				reinterpret_cast<const void*>(fmt.offset));


			ogl::enable_vertex_attrib_array(i);

			++i;
		}

		if (m_mesh->get_index_count() > 0)
		{
			ogl::bind_buffer(m_mesh->get_ibo(), ogl::BufferType::Element);
		}
	}

	const RenderObjectData& get_render_instance() const
	{
		return m_instance;
	}

	MeshData& get_mesh_data() override
	{
		if (!m_mesh_data)
		{
			m_mesh_data.reset(new MeshData);
		}

		return *m_mesh_data;
	}

	void update_mesh_data() override
	{
		if (m_mesh_data)
		{
			update_mesh_data(*m_mesh_data);
		}
	}

	void set_vertex_draw_mode(::VertexDrawMode mode) override
	{
		m_instance.draw_mode = translate_draw_mode(mode);
	}

	static ogl::BufferUsage translate_buffer_usage(VertexBufferUsage usage)
	{
		switch (usage)
		{
		case VertexBufferUsage::Static: return ogl::BufferUsage::Static;
		case VertexBufferUsage::Dynamic: return ogl::BufferUsage::Dynamic;
		case VertexBufferUsage::Streaming: return ogl::BufferUsage::Stream;
		}

		return ogl::BufferUsage::Static;
	}

	static ogl::VertexDrawMode translate_draw_mode(::VertexDrawMode mode)
	{
		switch (mode)
		{
		case ::VertexDrawMode::Line: return ogl::VertexDrawMode::Lines;
		case ::VertexDrawMode::LineStrip: return ogl::VertexDrawMode::LineStrip;
		case ::VertexDrawMode::Point: return ogl::VertexDrawMode::Points;
		case ::VertexDrawMode::Triangle: return ogl::VertexDrawMode::Triangles;
		case ::VertexDrawMode::TriangleFan: return ogl::VertexDrawMode::TriangleFan;
		case ::VertexDrawMode::TriangleStrip: return ogl::VertexDrawMode::TriangleStrip;
		}

		return ogl::VertexDrawMode::Triangles;
	}

	void set_vertex_buffer_usage(VertexBufferUsage usage) override
	{
		m_buffer_usage = translate_buffer_usage(usage);
	}

	void set_render_state(uint32_t flag) override
	{
		m_instance.flags |= flag;
	}

private:
	ResourceStorage* m_rs;
	BufferUsage m_buffer_usage;
	std::shared_ptr<StaticMesh> m_mesh;
	std::shared_ptr<ogl::ShaderProgram> m_program;
	std::shared_ptr<Texture> m_texture;
	std::unique_ptr<MeshData> m_mesh_data;
	RenderObjectData m_instance;
};

float quad_vertices[] = { 
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
};

class OpenGLRender final : public IRender
{
public:
	OpenGLRender()
		:m_width(0)
		,m_height(0)
		,m_fbo(0)
		,m_texture_color_buffer(0)
		,m_screen_quad_vao(0)
		,m_screen_quad_vbo(0)
		,m_post_program(nullptr)
		,m_rs(nullptr)
		,m_mvp_uniform(0)
	{
	}

	~OpenGLRender() override;

	bool init(ResourceStorage* rs, int width, int height) override
	{
		if (glewInit() != GLEW_OK)
		{
			return false;
		}

		m_width = width;
		m_height = height;
		m_rs = rs;

		const char* gl_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));

		LOG("gl version %s", gl_version);

		glViewport(0, 0, width, height);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClearDepth(1.0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable( GL_CULL_FACE );
		glCullFace(GL_BACK);
		

		init_fbo();

		return true;
	}

	void clear() override
	{
		if (m_fbo > 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

			glEnable(GL_DEPTH_TEST);
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void present() override
	{
		present_objects();

		present_color_texture();

		glBindVertexArray(0);
	}

	void present_objects()
	{
		while (!m_objects_to_present_data.empty())
		{
			auto instance = m_objects_to_present_data.back();
			m_objects_to_present_data.pop_back();

			draw_instance(instance);
		}
	}

	void apply_state_flags(uint32_t flags)
	{
		if (flags == 0)
		{
			glEnable(GL_BLEND);
			glEnable(GL_CULL_FACE);
			return;
		}

		if ((flags & RSF_BLEND) > 0)
		{
			glDisable(GL_BLEND);
			m_current_state |= RSF_BLEND;
		}

		if ((flags & RSF_CULL_TEST) > 0)
		{
			glDisable(GL_CULL_FACE);
			m_current_state |= RSF_CULL_TEST;
		}
	}

	void draw_instance(const RenderObjectData& render_data)
	{
		auto program = render_data.program;

		apply_state_flags(render_data.flags);
		
		ogl::bind_vertex_array(render_data.vao);
		
		glUseProgram(program);

		if (render_data.texture)
		{
			glActiveTexture(GL_TEXTURE0);

			glBindTexture(GL_TEXTURE_2D, render_data.texture);
		}

		set_uniform(program, "MVP", render_data.mvp);
		set_uniform(program, "Color", render_data.color);
		set_uniform(program, "texture_sampler", 0);

		GLenum mode = static_cast<GLenum>(render_data.draw_mode);
		
		if (render_data.index_count > 0)
		{
			glDrawElements(mode, render_data.index_count * sizeof(short), GL_UNSIGNED_SHORT, nullptr);
		}
		else
		{
			glDrawArrays(mode, 0, render_data.vertex_count);
		}
	}

	IRenderObject* create_object() override
	{
		m_objects.push_back(NEW_OBJ(OpenGLRenderObject, m_rs));

		IRenderObject* obj = m_objects.back();

		return obj;
	}

	void delete_object(IRenderObject* obj) override
	{
		auto it = std::find(m_objects.begin(), m_objects.end(), obj);
		if (it != m_objects.end())
		{
			m_objects.erase(it);
		}
		DELETE_OBJ(obj);
	}

	void init_fbo()
	{
		if(!m_use_fbo)
		{
			return;
		}
		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		glGenTextures(1, &m_texture_color_buffer);
		glBindTexture(GL_TEXTURE_2D, m_texture_color_buffer);

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_color_buffer, 0);

		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenVertexArrays(1, &m_screen_quad_vao);
		glBindVertexArray(m_screen_quad_vao);
		ogl::BufferDescription desc;
		desc.buffer_type = BufferType::Array;
		desc.buffer_usage = BufferUsage::Static;
		desc.data = quad_vertices;
		desc.size = sizeof(float) * 24;
		m_screen_quad_vbo = ogl::create_buffer(desc).value_or(INVALID_HANDLE);
		
		glEnableVertexAttribArray( 0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindVertexArray(0);

		m_post_program = m_rs->get_resorce<ogl::ShaderProgram>("post_effect");
		ASSERT(m_post_program != nullptr);
	}

	void present_color_texture() const
	{
		if (m_fbo > 0)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST);

			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			glBindVertexArray(m_screen_quad_vao);
			const GLuint post_effect = m_post_program->get_handle();
			glUseProgram(post_effect);

			set_uniform(post_effect, "width", m_width);
			set_uniform(post_effect, "height", m_height);

			glBindTexture(GL_TEXTURE_2D, m_texture_color_buffer);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	void add_to_frame(IRenderObject* object) override
	{
		const auto gl_obj = dynamic_cast<OpenGLRenderObject*>(object);

		add_to_frame(gl_obj->get_render_instance());
	}

	void add_to_frame(const RenderObjectData& data)
	{
		m_objects_to_present_data.push_back(data);
	}

private:
	bool m_use_fbo = true;
	GLint m_width;
	GLint m_height;
	GLuint m_fbo;
	GLuint m_texture_color_buffer;
	GLuint m_screen_quad_vao;
	GLuint m_screen_quad_vbo;
	std::shared_ptr<ogl::ShaderProgram> m_post_program;
	ResourceStorage* m_rs;
	GLint m_mvp_uniform;

	std::vector<OpenGLRenderObject*> m_objects;
	std::vector<OpenGLRenderObject*> m_objects_to_present;
	std::vector<RenderObjectData> m_objects_to_present_data;

	uint32_t m_current_state = 0;
};

IRender*
IRender::create()
{
	return NEW_OBJ(OpenGLRender);
}

OpenGLRender::~OpenGLRender()
{	
	for (auto obj : m_objects)
	{
		DELETE_OBJ(obj);
	}
	
	glDeleteVertexArrays(1, &m_screen_quad_vao);
	glDeleteBuffers(1, &m_screen_quad_vbo);
	glDeleteTextures(1, &m_texture_color_buffer);
	glDeleteFramebuffers(1, &m_fbo);
}
