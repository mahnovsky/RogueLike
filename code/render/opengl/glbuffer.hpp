#pragma once

#include "gltypes.hpp"
#include <cstdint>

namespace ogl
{
	class Buffer
	{
	public:
		Buffer();
		~Buffer();

		void init(BufferType buffer_type, BufferUsage buffer_usage, const void* data_ptr, std::uint32_t size);

		void update(const void* data_ptr, std::uint32_t size);

		void bind();

	private:
		std::uint32_t m_handle;
		BufferType m_buffer_type;
		BufferUsage m_buffer_usage;
	};
}