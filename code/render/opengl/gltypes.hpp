#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>

using std::uint32_t;
using std::int32_t;
using Handle = uint32_t;
using Result = std::optional<Handle>;

extern "C" {
#define GLEW_STATIC
#include <GL/glew.h>
}

#ifdef __WINDOWS__
#include <GL/GL.h>
#include <GL/wglew.h>
#include <windows.h>
#else
#include <GL/gl.h>
#endif

#define INVALID_HANDLE 0

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