#include "glshader.hpp"
#include "opengl_wrapper.hpp"

namespace ogl
{
	Shader::Shader(ShaderType type)
		:m_handle(OGL_INVALID_HANDLE)
		,m_type(type)
	{
	}

	Shader::~Shader()
	{
		LOG("OGL Shader destroyed");
		if (m_handle != OGL_INVALID_HANDLE)
		{
			delete_shader(m_handle);
		}
	}

	bool Shader::init(std::vector<uint8_t> data)
	{
		if (auto res = load_shader(m_type, data.data(), data.size()))
		{
			m_handle = res.value();

			return true;
		}
		
		return false;
	}

	std::string_view Shader::get_error_message() const
	{
		return get_shader_info(m_handle);
	}

	Program::Program()
		:m_handle()
	{
		if (auto res = ogl::create_program())
		{
			m_handle = res.value();
		}
	}

	Program::~Program()
	{
		delete_program(m_handle);
	}

	void Program::attach(const Shader& s)
	{
		attach_shader(m_handle, s.m_handle);
	}

	bool Program::link()
	{
		link_program(m_handle);

		return check_shader_link(m_handle);
	}

	uint32_t Program::get_handle() const
	{
		return m_handle;
	}
}
