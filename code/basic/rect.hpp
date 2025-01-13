#pragma once

#include "glm/glm.hpp"

struct Rect
{
	glm::vec2 pos; // left-bottom
	glm::vec2 size;

	Rect() = default;
	Rect(const glm::vec2& _size)
		: pos()
		, size(_size)
	{
	}

	Rect(const glm::vec2& _pos, const glm::vec2& _size)
		: pos(_pos)
		, size(_size)
	{
	}

	bool hit_test(const glm::vec2& point) const
	{
		glm::vec2 rt = pos + size;
		return point.x >= pos.x && point.x <= rt.x && point.y >= pos.y && point.y <= rt.y;
	}
};
