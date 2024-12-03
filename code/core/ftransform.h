#pragma once

#include "defines.hpp"

namespace core
{
	struct FTransform
	{
		static inline glm::vec3 FORWARD = { 0.f, 0.f, -1.f };
		static inline glm::vec3 UP = { 0.f, 1.f, 0.f };
		static inline glm::vec3 RIGHT = { 1.f, 0.f, 0.f };

		glm::vec3 pos;
		glm::vec3 scale = { 1.f, 1.f, 1.f };
		glm::vec3 rotation;
		glm::vec3 pivot;
		glm::mat4 transform;

		glm::vec3 get_world_position() const;
		glm::vec3 get_forward() const;
		glm::mat4 get_rotation() const;

		void update(const FTransform* parent);
	};

} // namespace core
