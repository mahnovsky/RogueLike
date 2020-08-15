#include "opengl_wrapper.hpp"

namespace ogl
{
	GLenum g_OpenGLError;

	uint32_t create_vertex_array()
	{
		GLuint vao;
		glGenVertexArrays(1, &vao);
		CHECK_LAST_CALL();

		return vao;
	}

	void delete_vertex_array(uint32_t vao)
	{
		glDeleteVertexArrays(1, &vao);
		CHECK_LAST_CALL();
	}

	void bind_vertex_array(uint32_t vao)
	{
		glBindVertexArray(vao);
		CHECK_LAST_CALL();
	}

	std::uint32_t create_buffer(
		BufferType buffer_type,
		BufferUsage buffer_usage, 
		const void* data, 
		std::uint32_t size)
	{
		std::uint32_t buffer;

		glGenBuffers(1, &buffer);
		CHECK_LAST_CALL();

		update_buffer(buffer, buffer_type, buffer_usage, data, size);

		return buffer;
	}

	void update_buffer(
		std::uint32_t handle,
		BufferType buffer_type,
		BufferUsage buffer_usage,
		const void* data,
		std::uint32_t size)
	{
		const uint32_t btype = static_cast<uint32_t>(buffer_type);
		bind_buffer(handle, static_cast<BufferType>(btype));

		const GLenum busage = static_cast<GLenum>(buffer_usage);
		glBufferData(btype, size, data, busage);
		CHECK_LAST_CALL();
	}

	void delete_buffer(std::uint32_t handle)
	{
		glDeleteBuffers(1, &handle);
		CHECK_LAST_CALL();
	}

	void bind_buffer(std::uint32_t handle, BufferType buffer_type)
	{
		glBindBuffer(static_cast<GLenum>(buffer_type), handle);
		CHECK_LAST_CALL();
	}

	void vertex_attrib_pointer(uint32_t index, uint32_t size, uint32_t type, uint32_t is_normalized, uint32_t stride, const void* ptr)
	{
		glVertexAttribPointer( index, size, type, is_normalized, stride, ptr);
		CHECK_LAST_CALL();
	}

	void enable_vertex_attrib_array(uint32_t index)
	{
		glEnableVertexAttribArray(index);
		CHECK_LAST_CALL();
	}

	GLuint create_program()
	{
		GLuint handle = glCreateProgram();
		CHECK_LAST_CALL();

		return handle;
	}

	void delete_program(GLuint program)
	{
		glDeleteProgram(program);
		CHECK_LAST_CALL();
	}

	GLuint create_shader(ShaderType type)
	{
		GLuint handle = glCreateShader(static_cast<GLenum>(type));
		CHECK_LAST_CALL();

		return handle;
	}

	void delete_shader(GLuint handle)
	{
		glDeleteShader(handle);
		CHECK_LAST_CALL();
	}

	// --------- shaders --------
	GLuint compile_shader(ShaderType type, void* data_ptr, uint32_t size)
	{
		ASSERT(data_ptr && size > 0);

		GLuint handle = create_shader(type);

		GLchar* data = static_cast<GLchar*>(data_ptr);
		GLint gsize = static_cast<GLint>(size);

		glShaderSource(handle, 1, &data, &gsize);
		CHECK_LAST_CALL();

		glCompileShader(handle);
		CHECK_LAST_CALL();

		return handle;
	}

	void attach_shader(uint32_t program, uint32_t shader)
	{
		glAttachShader(program, shader);
		CHECK_LAST_CALL();
	}

	void link_program(uint32_t programm)
	{
		glLinkProgram(programm);
		CHECK_LAST_CALL();
	}

	std::string_view get_shader_info(GLuint shader)
	{
		static GLchar infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);

		return infoLog;
	}

	bool is_shader_compiled(GLuint shader)
	{
		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		CHECK_LAST_CALL();

		return success > 0;
	}

	bool check_shader_link(GLuint shader)
	{
		GLint success;
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		CHECK_LAST_CALL();

		return success > 0;
	}

	std::int32_t get_uniform(std::uint32_t program, const char* name)
	{
		std::int32_t result = glGetUniformLocation(program, name);

		CHECK_LAST_CALL();

		return result;
	}

	void set_uniform(std::uint32_t program, const char* name, const glm::vec2& v)
	{
		std::int32_t pos = get_uniform(program, name);

		glUniform2f(pos, v.x, v.y);
		CHECK_LAST_CALL();
	}

	void set_uniform(std::uint32_t program, const char* name, const basic::Color& color)
	{
		std::int32_t pos = get_uniform(program, name);

		glm::vec4 c{ static_cast<float>(color.red) / 255,
			static_cast<float>(color.blue) / 255,
			static_cast<float>(color.green) / 255,
			static_cast<float>(color.alpha) / 255 };

		glUniform4fv(pos, 1, glm::value_ptr(c));
		CHECK_LAST_CALL();
	}

	void set_uniform(std::uint32_t program, const char* name, const glm::mat4& mat)
	{
		std::int32_t pos = get_uniform(program, name);

		glUniformMatrix4fv(pos, 1, GL_FALSE, glm::value_ptr(mat));
		CHECK_LAST_CALL();
	}

	void set_uniform(std::uint32_t program, const char* name, std::int32_t v)
	{
		std::int32_t pos = get_uniform(program, name);

		glUniform1i(pos, v);
		CHECK_LAST_CALL();
	}
}