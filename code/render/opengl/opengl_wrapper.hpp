#pragma once

#include "gltypes.hpp"
#include "defines.hpp"
#include "glm/glm.hpp"

#include <optional>

namespace ogl
{

extern GLenum g_OpenGLError;

#define BREAK_ON_ERROR 1

bool is_call_success();

#define OPENGL_CHECK_FOR_ERRORS() \
	if ((g_OpenGLError = glGetError()) != GL_NO_ERROR) \
	{\
		LOG("OpenGL error 0x%X\n", (unsigned)g_OpenGLError); \
		g_OpenGLError = GL_NO_ERROR; \
		if(BREAK_ON_ERROR) \
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
	struct BufferDescription
	{
		BufferType buffer_type;
		BufferUsage buffer_usage;
		const void* data;
		uint32_t size;
	};

	Result create_vertex_array();

	void delete_vertex_array(Handle vao);

	void bind_vertex_array(Handle vao);

	Result create_buffer(const BufferDescription& desc);

	bool update_buffer(Handle handle, const BufferDescription& desc);

	void delete_buffer(Handle handle);

	void bind_buffer(Handle handle, BufferType buffer_type);

	void vertex_attrib_pointer( 
		uint32_t index, 
		uint32_t size, 
		uint32_t type, 
		uint32_t is_normalized,
		uint32_t stride, 
		const void* ptr);

	void enable_vertex_attrib_array(uint32_t index);

	// ----------------- opengl shader functions ---------------------

	Result create_program();

	void use_program(Handle program);

	void delete_program(Handle program);

	Result create_shader(ShaderType type);

	void delete_shader(Handle handle);

	bool attach_source(Handle handle, void* data_ptr, uint32_t size);

	bool compile_shader(Handle handle);

	Result load_shader(ShaderType type, void* data_ptr, uint32_t size);

	void attach_shader(Handle program, Handle shader);

	void link_program(Handle programm);

	std::string_view get_shader_info(Handle shader);

	bool is_shader_compiled(Handle shader);

	bool check_shader_link(Handle shader);

	int32_t get_uniform(Handle program, const char* name);

	void set_uniform(Handle program, const char* name, const glm::vec2& v);

	void set_uniform(Handle program, const char* name, const basic::Color& color);

	void set_uniform(Handle program, const char* name, const glm::mat4& mat);

	void set_uniform(Handle program, const char* name, std::int32_t v);

	void bind_texture(uint32_t target, Handle texture);

	void active_texture(uint32_t sample);
}