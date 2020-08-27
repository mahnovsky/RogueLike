#include "glbuffer.hpp"
#include "opengl_wrapper.hpp"

namespace ogl
{
	Buffer::Buffer()
		: m_handle(INVALID_HANDLE)
		, m_buffer_type(BufferType::Array)
		, m_buffer_usage(BufferUsage::Static)
	{
	}

	Buffer::~Buffer()
	{
		if (m_handle != INVALID_HANDLE)
		{
			delete_buffer(m_handle);
		}
	}

	bool Buffer::init_raw(BufferType buffer_type, BufferUsage buffer_usage, const void* data_ptr, std::uint32_t size)
	{
		m_buffer_type = buffer_type;
		m_buffer_usage = buffer_usage;

		BufferDescription desc;
		desc.buffer_type = buffer_type;
		desc.buffer_usage = buffer_usage;
		desc.data = data_ptr;
		desc.size = size;

		auto res = create_buffer(desc);
		if(res.has_value())
		{
			m_handle = res.value();
		}

		return res.has_value();
	}

	void Buffer::update(const void* data_ptr, std::uint32_t size)
	{
		BufferDescription desc;
		desc.buffer_type = m_buffer_type;
		desc.buffer_usage = m_buffer_usage;
		desc.data = data_ptr;
		desc.size = size;

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