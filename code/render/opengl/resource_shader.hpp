#pragma once

#include "defines.hpp"
#include "resource_storage.hpp"

#include "glshader.hpp"

namespace ogl
{
	class BaseShader : public core::FileResource
	{
	public:
		BaseShader(const char* file);
		~BaseShader() override = default;

		bool load(core::ResourceStorage* storage);

		const ogl::Shader& get_shader() const;

	private:
		ogl::Shader m_shader;
	};

	class ShaderProgram : public core::FileResource
	{
	public:
		ShaderProgram(const char* file);

		bool load(core::ResourceStorage* storage);

		uint32_t get_handle() const;

	private:

		bool init(std::shared_ptr<BaseShader> vertex, std::shared_ptr<BaseShader> fragment);

	private:
		ogl::Program m_program;

		std::shared_ptr<BaseShader> m_vertex_shader;
		std::shared_ptr<BaseShader> m_fragment_shader;
	};

}
