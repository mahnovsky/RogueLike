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

	void Buffer::init_raw(BufferType buffer_type, BufferUsage buffer_usage, const void* data_ptr, std::uint32_t size)
	{
		m_buffer_type = buffer_type;
		m_buffer_usage = buffer_usage;

		m_handle = create_buffer(buffer_type, buffer_usage, data_ptr, size);
	}

	void Buffer::update(const void* data_ptr, std::uint32_t size)
	{
		update_buffer(m_handle, m_buffer_type, m_buffer_usage, data_ptr, size);
	}

	void Buffer::bind()
	{
		bind_buffer(m_handle, m_buffer_type);
	}
	
	uint32_t Buffer::get_handle() const
	{
		return m_handle;
	}
}