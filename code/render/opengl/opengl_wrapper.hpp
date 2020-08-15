#pragma once

#include "gltypes.hpp"
#include "defines.hpp"
#include "glm/glm.hpp"

namespace ogl
{
extern GLenum g_OpenGLError;

#define BREAK_ON_ERROR 1

#define OPENGL_CHECK_FOR_ERRORS() \
	if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
	{\
		LOG("OpenGL error 0x%X\n", (unsigned)g_OpenGLError); \
		g_OpenGLError = GL_NO_ERROR; \
		ASSERT(false); \
	}


#define CHECK_LAST_CALL() \
	if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
	{\
		LOG("OpenGL error 0x%X, line %d\n", (unsigned)g_OpenGLError, __LINE__); \
		if(BREAK_ON_ERROR) \
		ASSERT(false); \
		g_OpenGLError = GL_NO_ERROR; \
	}

// --------- opengl buffer functions ------------
	uint32_t create_vertex_array();

	void delete_vertex_array(uint32_t vao);

	void bind_vertex_array(uint32_t vao);

	uint32_t create_buffer(
		BufferType buffer_type,
		BufferUsage buffer_usage,
		const void* data,
		std::uint32_t size);

	void update_buffer(
		std::uint32_t handle,
		BufferType buffer_type,
		BufferUsage buffer_usage,
		const void* data,
		std::uint32_t size);

	void delete_buffer(std::uint32_t handle);

	void bind_buffer(std::uint32_t handle, BufferType buffer_type);

	void vertex_attrib_pointer( uint32_t index, 
		uint32_t size, 
		uint32_t type, 
		uint32_t is_normalized,
		uint32_t stride, 
		const void* ptr);

	void enable_vertex_attrib_array(uint32_t index);

	// ----------------- opengl shader functions ---------------------

	GLuint create_program();

	void delete_program(GLuint program);

	GLuint create_shader(ShaderType type);

	void delete_shader(GLuint handle);

	GLuint compile_shader(ShaderType type, void* data_ptr, uint32_t size);

	void attach_shader(uint32_t program, uint32_t shader);

	void link_program(uint32_t programm);

	std::string_view get_shader_info(GLuint shader);

	bool is_shader_compiled(GLuint shader);

	bool check_shader_link(GLuint shader);

	int32_t get_uniform(uint32_t program, const char* name);

	void set_uniform(uint32_t program, const char* name, const glm::vec2& v);

	void set_uniform(uint32_t program, const char* name, const basic::Color& color);

	void set_uniform(uint32_t program, const char* name, const glm::mat4& mat);

	void set_uniform(uint32_t program, const char* name, std::int32_t v);
}