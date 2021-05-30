#include "glbuffer.hpp"
#include "opengl_wrapper.hpp"

namespace ogl
{
	Buffer::Buffer()
		: m_handle(OGL_INVALID_HANDLE)
		, m_element_count(0)
		, m_element_size(0)
		, m_buffer_type(BufferType::Array)
		, m_buffer_usage(BufferUsage::Static)
	{
	}

	Buffer::~Buffer()
	{
		if (m_handle != OGL_INVALID_HANDLE)
		{
			delete_buffer(m_handle);
		}
	}

	bool Buffer::init_raw(BufferType buffer_type, BufferUsage buffer_usage, const void* data_ptr, uint32_t count, uint32_t item_size)
	{
		m_buffer_type = buffer_type;
		m_buffer_usage = buffer_usage;
		m_element_count = count;
		m_element_size = item_size;

		BufferDescription desc;
		desc.buffer_type = buffer_type;
		desc.buffer_usage = buffer_usage;
		desc.data = data_ptr;
		desc.size = item_size * count;

		auto res = create_buffer(desc);
		if (res.has_value())
		{
			m_handle = res.value();

			return true;
		}

		return false;
	}

	void Buffer::update(const void* data_ptr, std::uint32_t count)
	{
		BufferDescription desc;
		desc.buffer_type = m_buffer_type;
		desc.buffer_usage = m_buffer_usage;
		desc.data = data_ptr;
		desc.size = count * m_element_size;
		desc.submit = count <= m_element_count;
		m_element_count = count;

		update_buffer(m_handle, desc);
	}

	void Buffer::bind()
	{
		bind_buffer(m_handle, m_buffer_type);
	}
	
	uint32_t Buffer::get_handle() const
	{
		return m_handle;
	}

	uint32_t Buffer::get_element_count() const
	{
		return m_element_count;
	}
}