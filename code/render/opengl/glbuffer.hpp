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

		template <class ItemType>
		bool init(BufferType buffer_type, BufferUsage buffer_usage, const ItemType* data_ptr, uint32_t count)
		{
			m_element_count = count;
			return init_raw(buffer_type, buffer_usage, data_ptr, count, sizeof(ItemType));
		}

		bool init_raw(BufferType buffer_type, BufferUsage buffer_usage, const void* data_ptr, uint32_t count, uint32_t item_size);

		void update(const void* data_ptr, uint32_t size);

		void bind();

		uint32_t get_handle() const;

		uint32_t get_element_count() const;

	private:
		Handle m_handle;
		uint32_t m_element_count;
		BufferType m_buffer_type;
		BufferUsage m_buffer_usage;
	};
}