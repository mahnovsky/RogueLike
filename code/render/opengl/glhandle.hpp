#pragma once

#include "gltypes.hpp"

namespace ogl
{
	enum class HandleType
	{
		VertexArray,
		VertexBuffer,
		Texture,
		Shader,
		Program
	};

	template <class T>
	class Handle final
	{
	public:
		Handle(HandleType type);
		~Handle();

	private:
		uint32_t m_handle;
	};
}