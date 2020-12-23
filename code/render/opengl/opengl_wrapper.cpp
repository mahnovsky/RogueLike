#include "opengl_wrapper.hpp"
#ifdef _DEBUG
	#define OGL_LOG(...) LOG(__VA_ARGS__)
#else
	#define OGL_LOG(...)
#endif

namespace ogl
{
	GLenum g_OpenGLError;

	bool is_call_success()
	{
		bool result = glGetError() == GL_NO_ERROR;

		if (!result && BREAK_ON_ERROR)
		{
			LOG("OpenGL error 0x%X\n", (unsigned)g_OpenGLError);
			ASSERT(false);
		}

		return result;
	}

	Result create_vertex_array()
	{
		GLuint vao;
		glGenVertexArrays(1, &vao);
		
		if (is_call_success())
		{
			OGL_LOG("Create vao %d", vao);
			return vao;
		}

		return {};
	}

	void delete_vertex_array(Handle vao)
	{
		OGL_LOG("Delete vao %d", vao);

		glDeleteVertexArrays(1, &vao);
		CHECK_LAST_CALL();
	}

	void bind_vertex_array(Handle vao)
	{
		bool err = glGetError() == GL_NO_ERROR;
		ASSERT(err);
		glBindVertexArray(vao);
		CHECK_LAST_CALL();
	}

	Result create_buffer(const BufferDescription& desc)
	{
		uint32_t buffer;

		glGenBuffers(1, &buffer);
		if (is_call_success() && update_buffer(buffer, desc))
		{
			OGL_LOG("ogl buffer created %d", buffer);
			return buffer;
		}

		return {};
	}

	bool update_buffer(Handle handle, const BufferDescription& desc)
	{
		bind_buffer(handle, static_cast<BufferType>(desc.buffer_type));

		const uint32_t btype = static_cast<uint32_t>(desc.buffer_type);
		const GLenum busage = static_cast<GLenum>(desc.buffer_usage);

		glBufferData(btype, desc.size, desc.data, busage);

		return is_call_success();
	}

	void delete_buffer(Handle handle)
	{
		OGL_LOG("Delete ogl buffer %d", handle);

		glDeleteBuffers(1, &handle);
		CHECK_LAST_CALL();
	}

	void bind_buffer(Handle handle, BufferType buffer_type)
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

	Result create_program()
	{
		GLuint handle = glCreateProgram();
		if (is_call_success())
		{
			return handle;
		}

		return {};
	}

	void use_program(Handle program)
	{
		glUseProgram(program);
		CHECK_LAST_CALL();
	}

	void delete_program(Handle program)
	{
		OGL_LOG("Delete ogl program %d", program);

		glDeleteProgram(program);
		CHECK_LAST_CALL();
	}

	Result create_shader(ShaderType type)
	{
		Handle handle = glCreateShader(static_cast<GLenum>(type));
		if (is_call_success())
		{
			return handle;
		}

		return {};
	}

	void delete_shader(Handle handle)
	{
		OGL_LOG("Delete ogl shader %d", handle);

		glDeleteShader(handle);
		CHECK_LAST_CALL();
	}

	// --------- shaders --------
	bool attach_source(Handle handle, void* data_ptr, uint32_t size)
	{
		GLchar* data = static_cast<GLchar*>(data_ptr);
		GLint gsize = static_cast<GLint>(size);

		glShaderSource(handle, 1, &data, &gsize);
		
		return is_call_success();
	}

	bool compile_shader(Handle handle)
	{
		glCompileShader(handle);

		return is_shader_compiled(handle);
	}

	Result load_shader(ShaderType type, void* data_ptr, uint32_t size)
	{
		if (size == 0 || data_ptr == nullptr)
		{
			LOG("Failed to load shader, invalid arguments");
			return {};
		}

		if (auto res = create_shader(type))
		{
			Handle handle = res.value();
			if(attach_source(res.value(), data_ptr, size) && compile_shader(res.value()))
			{
				return handle;
			}
		}

		return {};
	}

	void attach_shader(Handle program, Handle shader)
	{
		glAttachShader(program, shader);
		CHECK_LAST_CALL();
	}

	void link_program(Handle programm)
	{
		glLinkProgram(programm);
		CHECK_LAST_CALL();
	}

	std::string_view get_shader_info(Handle shader)
	{
		static GLchar infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);

		return infoLog;
	}

	bool is_shader_compiled(Handle shader)
	{
		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		CHECK_LAST_CALL();

		return success > 0;
	}

	bool check_shader_link(Handle shader)
	{
		GLint success;
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		CHECK_LAST_CALL();

		return success > 0;
	}

	std::int32_t get_uniform(Handle program, const char* name)
	{
		std::int32_t result = glGetUniformLocation(program, name);

		CHECK_LAST_CALL();

		return result;
	}

	void set_uniform(Handle program, const char* name, const glm::vec2& v)
	{
		std::int32_t pos = get_uniform(program, name);

		glUniform2f(pos, v.x, v.y);
		CHECK_LAST_CALL();
	}

	void set_uniform(Handle program, const char* name, const basic::Color& color)
	{
		std::int32_t pos = get_uniform(program, name);

		glm::vec4 c{ 
			static_cast<float>(color.red) / 255,
			static_cast<float>(color.green) / 255,
			static_cast<float>(color.blue) / 255,
			static_cast<float>(color.alpha) / 255 };

		glUniform4fv(pos, 1, glm::value_ptr(c));
		CHECK_LAST_CALL();
	}

	void set_uniform(Handle program, const char* name, const glm::mat4& mat)
	{
		std::int32_t pos = get_uniform(program, name);

		glUniformMatrix4fv(pos, 1, GL_FALSE, glm::value_ptr(mat));
		CHECK_LAST_CALL();
	}

	void set_uniform(Handle program, const char* name, std::int32_t v)
	{
		std::int32_t pos = get_uniform(program, name);

		glUniform1i(pos, v);
		CHECK_LAST_CALL();
	}

	void bind_texture(uint32_t target, Handle texture)
	{
		glBindTexture(target, texture);
		CHECK_LAST_CALL();
	}

	void active_texture(uint32_t texture)
	{
		glActiveTexture(texture);
		CHECK_LAST_CALL();
	}
}