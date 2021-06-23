#include "resource_shader.hpp"

#include "GL/glew.h"
#include "file.hpp"
#include "config.hpp"

#include "render_common.hpp"

namespace ogl
{
	ShaderType resolve_shader_type(const char* file)
	{
		std::string filename = file;
		auto delimiter = filename.find_last_of('.');
		if (delimiter == std::string::npos && (delimiter + 2) < filename.size())
		{
			return ShaderType::Invalid;
		}

		std::string ext = filename.substr(delimiter + 1);

		if (ext == "vs")
		{
			return ShaderType::Vertex;
		}
		if (ext == "fs")
		{
			return ShaderType::Fragment;
		}
		
		return ShaderType::Invalid;
	}

	BaseShader::BaseShader(const char* file)
		: FileResource(file)
		, m_shader(resolve_shader_type(file))
	{	
	}

	bool BaseShader::load(core::ResourceStorage*)
	{
		auto file = get_file_name();
		ASSERT(!file.empty());

		auto data = core::get_file_content(file.data());

		if (!data.empty())
		{
			if (!m_shader.init(std::move(data)))
			{
				LOG("Failed compile shader: %s", m_shader.get_error_message().data());
			}
		}
		else
		{
			LOG("Failed to load shader file: %s", file.data());
		}

		return true;
	}

	const ogl::Shader& BaseShader::get_shader() const
	{
		return m_shader;
	}


	ShaderProgram::ShaderProgram(const char* file)
		: FileResource(file)
		, m_vertex_shader(nullptr)
		, m_fragment_shader(nullptr)
	{
	}

	bool ShaderProgram::load(core::ResourceStorage* storage)
	{
		std::shared_ptr<Config> config = storage->get_resorce< Config >("shaders/programs.conf");

		if (config)
		{
			const basic::JsonObject* shader_conf = config->get_values(get_file_name().data());

			ASSERT(shader_conf);

			std::string vertex_file;
			shader_conf->get("vertex_shader", vertex_file);

			if (vertex_file.empty())
			{
				return false;
			}

			std::string fragment_file;
			shader_conf->get("fragment_shader", fragment_file);

			if (fragment_file.empty())
			{
				return false;
			}

			std::shared_ptr<BaseShader> vertex = storage->get_resorce< BaseShader >(vertex_file.c_str());
			std::shared_ptr<BaseShader> fragment = storage->get_resorce< BaseShader >(fragment_file.c_str());

			return init(vertex, fragment);
		}

		return false;
	}

	uint32_t ShaderProgram::get_handle() const
	{
		return m_program.get_handle();
	}

	bool ShaderProgram::init(std::shared_ptr<BaseShader> vertex, std::shared_ptr<BaseShader> fragment)
	{
		if (vertex && fragment)
		{
			m_program.attach(vertex->get_shader());
			m_program.attach(fragment->get_shader());

			if (m_program.link())
			{
				m_vertex_shader = vertex;
				m_fragment_shader = fragment;

				return true;
			}
		}
		LOG("Failed link shader program");

		return false;
	}
}
