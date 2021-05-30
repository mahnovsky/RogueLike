#include "opengl_render.hpp"
#include "file.hpp"
#include "vector.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "camera.hpp"
#include "transform.hpp"
#include "window.hpp"
#include "render_system.hpp"
#include "resource_storage.hpp"
#include "texture.hpp"
#include "opengl/resource_shader.hpp"
#include "static_mesh.hpp"
#include "pool.hpp"

#include "opengl_wrapper.hpp"

using namespace ogl;

extern "C" {
#define GLEW_STATIC
#include <GL/glew.h>
}

#include <GL/gl.h>

#define VIEW_PROJECT_UNIFORM "ViewProjection"
#define MODEL_UNIFORM "Model"
#define TEXTURE_SAMPLER_UNIFORM "texture_sampler"
#define COLOR_UNIFORM "Color"

//GLenum g_OpenGLError = GL_NO_ERROR;

struct RenderObjectData
{
	ogl::VertexDrawMode draw_mode = ogl::VertexDrawMode::Triangles;
	GLuint vao;
	GLuint texture;
	GLuint program;
	GLuint element_count;
	basic::Color color;
	glm::mat4 mvp;
	uint32_t flags;
	uint32_t camera_index = 0;
};

class OpenGLRenderObject final : public IRenderObject
{
public:
	OpenGLRenderObject() = delete;

	OpenGLRenderObject(core::ResourceStorage* rs)
		: m_rs(rs)
		, m_mesh(nullptr)
		, m_program(nullptr)
		, m_texture(nullptr)
		, m_instance()
		, m_buffer_usage(BufferUsage::Static)
	{
		memset(&m_instance, 0, sizeof(RenderObjectData));
		m_instance.draw_mode = ogl::VertexDrawMode::Triangles;
		m_instance.flags = 0;
		m_instance.mvp = glm::mat4(1.f);
		m_instance.color = {255,255,255,255};
	}

	~OpenGLRenderObject() override
	{
		if (m_instance.vao != OGL_INVALID_HANDLE)
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
			if (m_instance.vao != OGL_INVALID_HANDLE)
			{
				ogl::delete_vertex_array(m_instance.vao);
				m_instance.vao = OGL_INVALID_HANDLE;
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

	void update_mesh_data(const MeshData& data)
	{
		if (m_instance.vao == OGL_INVALID_HANDLE)
		{
			auto res = ogl::create_vertex_array();

			m_instance.vao = res.value_or(OGL_INVALID_HANDLE);
		}

		if (m_instance.vao != OGL_INVALID_HANDLE)
		{
			if (!m_mesh)
			{
				m_mesh = std::make_shared<StaticMesh>("mesh");
				if (m_mesh->init(data, m_buffer_usage))
				{
					setup_buffers();
				}
			}
			else
			{
				ogl::bind_vertex_array(m_instance.vao);

				m_mesh->update(data);
				setup_element_cout();
			}
		}
	}

	void setup_element_cout()
	{
		if (m_mesh->get_index_count() > 0)
		{
			m_instance.element_count = m_mesh->get_index_count();
			m_instance.flags |= ROF_INDEX_BUFFER;
		}
		else
		{
			m_instance.element_count = m_mesh->get_vertex_count();
			m_instance.flags &= ~ROF_INDEX_BUFFER;
		}
	}

	void setup_buffers()
	{
		setup_element_cout();

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

	void set_camera_index(uint32_t index) override
	{
		m_instance.camera_index = index;
	}

	uint32_t get_camera_index() const
	{
		return m_instance.camera_index;
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
	core::ResourceStorage* m_rs;
	BufferUsage m_buffer_usage;
	std::shared_ptr<StaticMesh> m_mesh;
	std::shared_ptr<ogl::ShaderProgram> m_program;
	std::shared_ptr<Texture> m_texture;
	std::unique_ptr<MeshData> m_mesh_data;
	RenderObjectData m_instance;
};

Pool<OpenGLRenderObject>* g_object_pool;

float quad_vertices[] = { 
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
};

OpenGLRender::OpenGLRender()
	:m_width(0)
	, m_height(0)
	, m_fbo(0)
	, m_texture_color_buffer(0)
	, m_screen_quad_vao(0)
	, m_screen_quad_vbo(0)
	, m_post_program(nullptr)
	, m_rs(nullptr)
	, m_mvp_uniform(0)
{
}

OpenGLRender::~OpenGLRender()
{
	m_objects.clear();
	if (g_object_pool)
	{
		DELETE_OBJ(g_object_pool);
	}
	if (m_fbo)
	{
		glDeleteVertexArrays(1, &m_screen_quad_vao);
		glDeleteBuffers(1, &m_screen_quad_vbo);
		glDeleteTextures(1, &m_texture_color_buffer);
		glDeleteFramebuffers(1, &m_fbo);
	}
}

bool OpenGLRender::init(core::ResourceStorage* rs, int width, int height)
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
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glClearDepth(1.0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	/*TODO:
	glEnable( GL_CULL_FACE );
	glCullFace(GL_BACK);*/


	init_fbo();

	return true;
}

void OpenGLRender::clear()
{
	if (m_fbo > 0)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

		glEnable(GL_DEPTH_TEST);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRender::present()
{
	if (!m_cams.empty())
	{
		present_objects();

		present_color_texture();

		ogl::bind_vertex_array(OGL_INVALID_HANDLE);
		m_current_vao = OGL_INVALID_HANDLE;
		ogl::use_program(OGL_INVALID_HANDLE);
		m_current_program = OGL_INVALID_HANDLE;
	}
}

void OpenGLRender::present_objects()
{
	for (auto obj : m_present_objects)
	{
		draw_instance(obj->get_render_instance());
	}
	m_present_objects.clear();
}

void OpenGLRender::apply_state_flags(uint32_t flags)
{
	std::pair<uint32_t, uint32_t> states[] = {
		{ROF_BLEND, GL_BLEND},
		{ROF_DEPTH_TEST, GL_DEPTH_TEST},
		{ROF_CULL_TEST, GL_CULL_FACE}
	};
	for (uint32_t i = 0; i < 3; ++i)
	{
		auto& state = states[i];
		((flags & state.first) > 0) ?
			glDisable(state.second) :
			glEnable(state.second);
	}
}

void OpenGLRender::draw_instance(const RenderObjectData& render_data)
{
	auto program = render_data.program;
	//apply_state_flags(render_data.flags);
	if (m_current_vao != render_data.vao)
	{
		ogl::bind_vertex_array(render_data.vao);
		m_current_vao = render_data.vao;
	}

	if (m_current_program != program)
	{
		ogl::use_program(program);
		m_current_program = program;

		update_view_projection_matrix(render_data);
	}

	if (render_data.camera_index != m_prev_camera_index)
	{
		update_view_projection_matrix(render_data);
	}

	if (render_data.texture)
	{
		active_texture(GL_TEXTURE0);
		bind_texture(GL_TEXTURE_2D, render_data.texture);
	}

	set_uniform(program, MODEL_UNIFORM, render_data.mvp);
	set_uniform(program, COLOR_UNIFORM, render_data.color);
	set_uniform(program, TEXTURE_SAMPLER_UNIFORM, 0);

	const GLenum mode = static_cast<GLenum>(render_data.draw_mode);

	if (render_data.flags & ROF_INDEX_BUFFER)
	{
		glDrawElements(mode, render_data.element_count, GL_UNSIGNED_SHORT, nullptr);
	}
	else
	{
		glDrawArrays(mode, 0, render_data.element_count);
	}

	CHECK_LAST_CALL();
}

IRenderObject* OpenGLRender::create_object()
{
	if (!g_object_pool)
	{
		g_object_pool = NEW_OBJ(Pool<OpenGLRenderObject>);
	}

	OpenGLRenderObject* render_obj = g_object_pool->alloc(m_rs);

	m_objects.push_back(render_obj);

	return render_obj;
}

void OpenGLRender::delete_object(IRenderObject* obj)
{
	OpenGLRenderObject* render_obj = dynamic_cast<OpenGLRenderObject*>(obj);
	if (render_obj && stdext::remove(m_objects, render_obj))
	{
		const uint32_t cam_index = obj->get_camera_index();
		stdext::remove(m_present_objects, render_obj);
		g_object_pool->free(render_obj);
	}
}

void OpenGLRender::init_fbo()
{
	if (!m_use_fbo)
	{
		return;
	}
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_texture_color_buffer);
	glBindTexture(GL_TEXTURE_2D, m_texture_color_buffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

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
	m_screen_quad_vbo = ogl::create_buffer(desc).value_or(OGL_INVALID_HANDLE);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

	m_post_program = m_rs->get_resorce<ogl::ShaderProgram>("post_effect");
	ASSERT(m_post_program != nullptr);
}

void OpenGLRender::present_color_texture() const
{
	if (m_fbo > 0)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ogl::bind_vertex_array(m_screen_quad_vao);
		const GLuint post_effect = m_post_program->get_handle();

		m_current_program = post_effect;
		ogl::use_program(post_effect);

		set_uniform(post_effect, "width", m_width);
		set_uniform(post_effect, "height", m_height);

		glBindTexture(GL_TEXTURE_2D, m_texture_color_buffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void OpenGLRender::add_to_frame(IRenderObject* object) 
{
	OpenGLRenderObject* ogl_object = static_cast<OpenGLRenderObject*>(
		static_cast<void*>(object));

	m_present_objects.push_back(ogl_object);
}

void OpenGLRender::add_camera(ICamera* camera)
{
	int32_t next_index = m_cams.size();
	m_cams.push_back(camera);
	camera->set_camera_index(next_index);
}

void OpenGLRender::update_view_projection_matrix(const RenderObjectData& render_data)
{
	ICamera* current_cam = m_cams[render_data.camera_index];
	glm::mat4 vp;
	current_cam->get_matrix(vp);
	set_uniform(render_data.program, VIEW_PROJECT_UNIFORM, vp);

	m_prev_camera_index = render_data.camera_index;
}

IRender* IRender::create()
{
	return NEW_OBJ(OpenGLRender);
}
