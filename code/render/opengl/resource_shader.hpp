#pragma once

#include "defines.hpp"
#include "resource_storage.hpp"

#include "glshader.hpp"

namespace ogl
{
	class BaseShader : public FileResource
	{
	public:
		BaseShader(const char* file);
		~BaseShader() override = default;

		bool load(ResourceStorage* storage);

		static BaseShader* create(const char* file);

		const ogl::Shader& get_shader() const;

	private:
		ogl::Shader m_shader;
	};

	class ShaderProgram : public FileResource
	{
	public:
		ShaderProgram(const char* file);

		bool load(ResourceStorage* storage);

		basic::uint32 get_handle() const;

		static ShaderProgram* create(const char* file);

	private:

		bool init(std::shared_ptr<BaseShader> vertex, std::shared_ptr<BaseShader> fragment);

	private:
		ogl::Program m_program;

		std::shared_ptr<BaseShader> m_vertex_shader;
		std::shared_ptr<BaseShader> m_fragment_shader;
	};

}
