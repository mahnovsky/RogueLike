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

extern "C" {
#define GLEW_STATIC
#include <GL/glew.h>
}

#include <GL/gl.h>

GLenum g_OpenGLError = GL_NO_ERROR;

struct RenderObjectData
{
	GLuint vao;
	GLuint texture;
	GLuint program;
	GLuint vertex_count;
	GLuint index_count;
	basic::Color color;
	glm::mat4 mvp;
};

void RenderObjectDataDelete::operator()(RenderObjectData* _Ptr) const noexcept { // delete a pointer

	delete _Ptr;
}

RenderObjectDataPtr create_render_data(DataPresentMode present_mode)
{
	RenderObjectDataPtr data{ new RenderObjectData, RenderObjectDataDelete() };

	return std::move(data);
}

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
	}

	~OpenGLRenderObject() override
	{
		glDeleteVertexArrays(1, &m_instance.vao);
	}

	void on_resource_changed(RenderResourceType type, const std::string& name) override
	{
		if (!name.empty())
		{
			apply_changes(type, name);
		}
	}

	void on_component_changed(RenderComponent& comp) override
	{
		for (basic::uint32 i = 0; i < enum2num(RenderResourceType::Count); ++i)
		{
			auto type = static_cast<RenderResourceType>(i);

			const std::string& name = comp.get_resource_name(type);

			if (!name.empty())
			{
				apply_changes(type, name);
			}
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
			OPENGL_CHECK_FOR_ERRORS();
			glGenVertexArrays(1, &m_instance.vao);
			CHECKED_CALL(glBindVertexArray, m_instance.vao);

			CHECKED_CALL(glBindBuffer, GL_ARRAY_BUFFER, m_mesh->get_vbo());

			auto fmt_list = m_mesh->get_fmt_list();
			basic::uint32 i = 0;
			for (const auto& fmt : fmt_list)
			{
				CHECKED_CALL(glVertexAttribPointer, i,
					static_cast<GLint>(fmt.size),
					fmt.type,
					fmt.is_normalized,
					sizeof(Vertex),
					reinterpret_cast<const void*>(fmt.offset));

				CHECKED_CALL(glEnableVertexAttribArray, i);

				++i;
			}

			CHECKED_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_mesh->get_ibo());

			m_instance.index_count = m_mesh->get_index_count();
			m_instance.vertex_count = m_mesh->get_vertex_count();
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

	const RenderObjectData& get_render_instance() const
	{
		return m_instance;
	}

private:
	ResourceStorage* m_rs;
	std::shared_ptr<StaticMesh> m_mesh;
	std::shared_ptr<ogl::ShaderProgram> m_program;
	std::shared_ptr<Texture> m_texture;
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
		OPENGL_CHECK_FOR_ERRORS();

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

			OPENGL_CHECK_FOR_ERRORS();
		}
	}

	void draw_instance(const RenderObjectData& render_data)
	{
		auto program = render_data.program;

		CHECKED_CALL(glBindVertexArray, render_data.vao);
		CHECKED_CALL(glUseProgram, program);

		if (render_data.texture)
		{
			CHECKED_CALL(glActiveTexture, GL_TEXTURE0);

			CHECKED_CALL(glBindTexture, GL_TEXTURE_2D, render_data.texture);
		}

		set_uniform(program, "MVP", render_data.mvp);
		set_uniform(program, "Color", render_data.color);

		if (render_data.index_count > 0)
		{
			CHECKED_CALL(glDrawElements, GL_TRIANGLES, render_data.index_count * sizeof(short), GL_UNSIGNED_SHORT, nullptr);
		}
		else
		{
			CHECKED_CALL(glDrawArrays, GL_TRIANGLES, 0, render_data.vertex_count);
		}
	}

	IRenderObject* create_object(RenderComponent& comp) override
	{
		m_objects.push_back(NEW_OBJ(OpenGLRenderObject, m_rs));

		IRenderObject* obj = m_objects.back();
		obj->on_component_changed(comp);

		return obj;
	}

	void delete_object(IRenderObject* obj) override
	{
		DELETE_OBJ(obj);
	}

	void init_fbo()
	{
		if(!m_use_fbo)
		{
			return;
		}
		CHECKED_CALL(glGenFramebuffers, 1, &m_fbo);
		CHECKED_CALL(glBindFramebuffer, GL_FRAMEBUFFER, m_fbo);

		CHECKED_CALL(glGenTextures, 1, &m_texture_color_buffer);
		CHECKED_CALL(glBindTexture, GL_TEXTURE_2D, m_texture_color_buffer);

		CHECKED_CALL(glTexImage2D, GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		CHECKED_CALL(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_color_buffer, 0);

		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		CHECKED_CALL(glGenVertexArrays, 1, &m_screen_quad_vao);
		CHECKED_CALL(glBindVertexArray, m_screen_quad_vao);
		m_screen_quad_vbo = create_buffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, quad_vertices, sizeof(float) * 24);
		OPENGL_CHECK_FOR_ERRORS();

		CHECKED_CALL(glEnableVertexAttribArray, 0);
		CHECKED_CALL(glVertexAttribPointer, 0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		CHECKED_CALL(glEnableVertexAttribArray, 1);
		CHECKED_CALL(glVertexAttribPointer, 1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		CHECKED_CALL(glBindVertexArray, 0);

		OPENGL_CHECK_FOR_ERRORS();
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

	void add_to_frame(const RenderObjectData& data) override
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
		delete_object(obj);
	}
	
	glDeleteVertexArrays(1, &m_screen_quad_vao);
	glDeleteBuffers(1, &m_screen_quad_vbo);
	glDeleteTextures(1, &m_texture_color_buffer);
	glDeleteFramebuffers(1, &m_fbo);
}
