#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>

using std::uint32_t;
using std::int32_t;
using Handle = uint32_t;
using Result = std::optional<Handle>;

#if defined(__WINDOWS__)
#include <Windows.h>
#endif

extern "C" {
#define GLEW_STATIC
//#include <winnt.h>
#include <GL/glew.h>
}

#if defined(__WINDOWS__)
#include <GL/GL.h>
#else
#include <GL/gl.h>
#endif

#define OGL_INVALID_HANDLE 0

namespace ogl
{
	enum class VertexDrawMode : GLenum
	{
		Points = GL_POINTS,
		Lines = GL_LINES,
		LineStrip = GL_LINE_STRIP,
		Triangles = GL_TRIANGLES,
		TriangleFan = GL_TRIANGLE_FAN,
		TriangleStrip = GL_TRIANGLE_STRIP
	};

	enum class BufferType : GLenum
	{
		Array = GL_ARRAY_BUFFER,
		Element = GL_ELEMENT_ARRAY_BUFFER
	};

	enum class BufferUsage : GLenum
	{
		Static = GL_STATIC_DRAW,
		Dynamic = GL_DYNAMIC_DRAW,
		Stream = GL_STREAM_DRAW
	};

	enum class ShaderType : GLenum
	{
		Invalid = 0,
		Vertex = GL_VERTEX_SHADER,
		Fragment = GL_FRAGMENT_SHADER
	};
}