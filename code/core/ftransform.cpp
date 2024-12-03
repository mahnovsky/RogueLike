#include "ftransform.h"

#include <glm/gtx/rotate_vector.hpp>

namespace core
{
	glm::vec3 FTransform::get_world_position() const
	{
		return glm::vec4 { pos, 1.f } * transform;
	}

	glm::vec3 FTransform::get_forward() const
	{
		const glm::mat4 rot = get_rotation();
		return glm::vec4 { FORWARD, 0.f } * rot;
	}

	glm::mat4 FTransform::get_rotation() const
	{
		glm::mat4 rot;
		rot = glm::rotate(rot, glm::radians(rotation.y), UP);
		rot = glm::rotate(rot, glm::radians(rotation.x), RIGHT);
		rot = glm::rotate(rot, glm::radians(rotation.z), FORWARD);

		return rot;
	}

	void FTransform::update(const FTransform* parent)
	{
		transform = get_rotation();
		transform = glm::scale(transform, scale);
		transform = glm::translate(transform, pos);

		if (parent) {
			transform = parent->transform * transform;
		}
	}
} // namespace core
