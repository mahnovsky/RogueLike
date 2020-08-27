#include "glhandle.hpp"
#include "opengl_wrapper.hpp"

namespace ogl
{
	Handle::Handle(HandleType type)
		:m_type(type)
		,m_handle(create(type))
	{
	}

	Handle::~Handle()
	{
		destroy(m_type, m_handle);
	}

	GLuint Handle::create(HandleType type)
	{
		switch (type)
		{
		case HandleType::VertexArray: 
		}
		return GLuint();
	}

	void Handle::destroy(HandleType type, uint32_t handle)
	{
	}

	void Handle::bind(HandleType type, uint32_t handle)
	{
	}
}