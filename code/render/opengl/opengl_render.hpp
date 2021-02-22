#pragma once

#include "render.hpp"
#include "resource_shader.hpp"

#include <memory>

struct RenderObjectData;
class OpenGLRenderObject;

class OpenGLRender final : public IRender
{
public:
	OpenGLRender();

	~OpenGLRender() override;

	bool init(core::ResourceStorage* rs, int width, int height) override;

	void clear() override;

	void present() override;

	void present_objects();

	void apply_state_flags(uint32_t flags);

	void draw_instance(const RenderObjectData& render_data);

	IRenderObject* create_object() override;

	void delete_object(IRenderObject* obj) override;

	void init_fbo();

	void present_color_texture() const;

	void add_to_frame(IRenderObject* object) override;

	int32_t add_camera(ICamera* camera) override;

	void update_view_projection_matrix(const RenderObjectData& render_data);

private:
	bool m_use_fbo = true;
	GLint m_width;
	GLint m_height;
	GLuint m_fbo;
	GLuint m_texture_color_buffer;
	GLuint m_screen_quad_vao;
	GLuint m_screen_quad_vbo;
	std::shared_ptr<ogl::ShaderProgram> m_post_program;
	core::ResourceStorage* m_rs;
	GLint m_mvp_uniform;
	mutable GLuint m_current_program = 0;
	GLuint m_current_vao = 0;

	std::vector<OpenGLRenderObject*> m_objects;
	std::vector<OpenGLRenderObject*> m_present_objects;
	std::vector<IRenderObject*> m_objects_to_destroy;

	uint32_t m_current_state = 0;

	std::vector<ICamera*> m_cams;
	uint32_t m_prev_camera_index = 0;
};
