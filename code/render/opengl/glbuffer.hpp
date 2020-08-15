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

		void init_raw(BufferType buffer_type, BufferUsage buffer_usage, const void* data_ptr, uint32_t size);

		template <class ItemType>
		void init(BufferType buffer_type, BufferUsage buffer_usage, ItemType* data_ptr, uint32_t count)
		{
			init_raw(buffer_type, buffer_usage, data_ptr, sizeof(ItemType) * count);
		}

		void update(const void* data_ptr, std::uint32_t size);

		void bind();

		uint32_t get_handle() const;

	private:
		uint32_t m_handle;
		BufferType m_buffer_type;
		BufferUsage m_buffer_usage;
	};
}