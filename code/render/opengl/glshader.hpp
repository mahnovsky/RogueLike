#pragma once

#include "gltypes.hpp"
#include <string_view>
#include <vector>

namespace ogl
{
	class Shader
	{
	public:
		Shader(ShaderType type);
		~Shader();

		bool init(std::vector<uint8_t> data);

		std::string_view get_error_message() const;

		friend class Program;
	private:
		Handle m_handle;
		ShaderType m_type;
	};

	class Program
	{
	public:
		Program();
		~Program();

		void attach(const Shader& s);

		bool link();

		uint32_t get_handle() const;

	private:
		Handle m_handle;
	};
}