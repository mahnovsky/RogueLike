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

class OpenGLRenderObject final : public IRenderObject
{
public:
	OpenGLRenderObject() = delete;

	OpenGLRenderObject(ResourceStorage* rs, basic::uint32 uid, basic::Vector<RenderInstance>& instances)
		: m_uid(uid)
		, m_instances(instances)
		, m_rs(rs)
		, m_mesh(nullptr)
		, m_program(nullptr)
		, m_texture(nullptr)
	{
	}

	~OpenGLRenderObject() override
	{
		m_instances[m_uid].vao = 0;
		SAFE_RELEASE(m_mesh);
		SAFE_RELEASE(m_texture);
		SAFE_RELEASE(m_program);
	}

	void on_resource_changed(RenderResourceType type, const basic::String& name) override
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
			auto type = static_cast<RenderResourceType>(i);

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

	void update_mesh(const basic::String& name)
	{
		if (m_mesh)
		{
			m_mesh->release();
		}

		m_mesh = m_rs->get_resorce<StaticMesh>(name.get_cstr(), m_mesh_settings);
		if (m_mesh)
		{
			RenderInstance& instance = m_instances[m_uid];

			if (m_mesh->get_index_count() > 0)
			{
				instance.draw_func = draw_insices;
				instance.element_count = m_mesh->get_index_count();
			}
			else if (m_mesh->get_vertex_count() > 0)
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
	}

	void update_program(const basic::String& name)
	{
		if (m_program)
		{
			m_program->release();
		}

		m_program = m_rs->get_resorce<ShaderProgram>(name.get_cstr());
		if (m_program)
		{
			m_program->retain();

			m_instances[m_uid].program = m_program->get_handle();
		}
	}

	void update_color(basic::Color color) override
	{
		m_instances[m_uid].color = color;
	}

	void update_texture(const basic::String& name)
	{
		if(m_texture)
		{
			m_texture->release();
		}

		m_texture = m_rs->get_resorce<Texture>(name.get_cstr());
		if (m_texture)
		{
			m_texture->retain();

			m_instances[m_uid].texture = m_texture->get_handle();
		}
	}

private:
	basic::uint32 m_uid;
	basic::Vector<RenderInstance>& m_instances;
	ResourceStorage* m_rs;
	StaticMesh* m_mesh;
	ShaderProgram* m_program;
	Texture* m_texture;
	MeshLoadSettings m_mesh_settings;
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
		: m_rs(nullptr)
		, m_mvp_uniform(0)
	{
	}

	~OpenGLRender() override;

	bool init(ResourceStorage* rs, int width, int height) override
	{
		if (glewInit() != GLEW_OK)
		{
			return false;
		}
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

		//glEnable( GL_CULL_FACE );
		//glCullFace(GL_FRONT);

		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		glGenTextures(1, &m_texture_color_buffer);
		glBindTexture(GL_TEXTURE_2D, m_texture_color_buffer);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_color_buffer, 0);

		unsigned int rbo;
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenVertexArrays(1, &m_screen_quad_vao);
		glBindVertexArray(m_screen_quad_vao);
		m_screen_quad_vbo = create_buffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, quad_vertices, sizeof(float) * 24);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

		glBindVertexArray(0);

		m_post_program = m_rs->get_resorce<ShaderProgram>("posteffect");
		ASSERT(m_post_program != nullptr);

		m_post_program->retain();

		m_width = width;
		m_height = height;

		return true;
	}

	void clear() override
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		glEnable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void present() override
	{
		for (auto& instance : m_instances)
		{
			glBindVertexArray(instance.vao);
			glUseProgram(instance.program);

			if(instance.texture > 0)
			{
				glActiveTexture(GL_TEXTURE0);

				glBindTexture(GL_TEXTURE_2D, instance.texture);
			}

			set_uniform(instance.program, "MVP", instance.mvp);
			set_uniform(instance.program, "Color", instance.color);

			instance.draw_func(instance);
		}
		glBindVertexArray(0);

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

		glUseProgram(0);
		glBindVertexArray(0);
	}

	IRenderObject* create_object(RenderComponent& comp) override
	{
		const basic::uint32 uid = m_instances.get_size();
		m_instances.push({ 0, 0, 0, 0, draw_dummy });

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
		auto internal_obj = dynamic_cast<OpenGLRenderObject*>(obj);
		if (m_objects.find_first(index, internal_obj))
		{
			m_objects.swap_remove(index);
			DELETE_OBJ(obj);
		}
	}

private:
	GLint m_width;
	GLint m_height;
	GLuint m_fbo;
	GLuint m_texture_color_buffer;
	GLuint m_screen_quad_vao;
	GLuint m_screen_quad_vbo;
	ShaderProgram* m_post_program;
	ResourceStorage* m_rs;
	GLint m_mvp_uniform;
	basic::Vector<OpenGLRenderObject*> m_objects;
	basic::Vector<RenderInstance> m_instances;
};

IRender*
IRender::create()
{
	return NEW_OBJ(OpenGLRender);
}

OpenGLRender::~OpenGLRender()
{
	SAFE_RELEASE(m_post_program);
}
