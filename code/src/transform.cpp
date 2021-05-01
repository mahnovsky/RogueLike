#include "transform.hpp"

#include "glm/ext.hpp"
#include "entity.hpp"
#include "octree.h"

#include <glm/gtc/quaternion.hpp>

Transform::Transform(Entity* ent)
	: Component(ent)
	, m_final_mat(1.f)
	, m_pos()
	, m_euler_angles()
	, m_pivot_point()
	, m_quat()
	, m_scale(1.f)
{
}

Transform::~Transform()
{
}

void
Transform::set_position(const glm::vec3& pos)
{
	m_pos = pos;
	
	m_entity->send_component_event(this, ComponentEvent::Updated);
	is_changed = true;
}

glm::vec3 Transform::get_position() const
{
	return m_pos;
}

void Transform::set_pivot_point(const glm::vec3& pivot)
{
	m_pivot_point = pivot;

	is_changed = true;
}

glm::vec3 Transform::get_pivot_point() const
{
	return m_pivot_point;
}

void Transform::set_euler_angles(const glm::vec3& angles)
{
	m_quat = glm::quat(angles);

	m_entity->send_component_event(this, ComponentEvent::Updated);
	is_changed = true;
}

glm::quat Transform::get_quaternion() const
{
	return m_quat;
}

void Transform::set_quaternion(glm::quat rotation)
{
	m_quat = rotation;
}

void Transform::set_scale(const glm::vec3& scale)
{
	m_scale = scale;

	m_entity->send_component_event(this, ComponentEvent::Updated);
	is_changed = true;
}

glm::vec3 Transform::get_scale() const
{
	return m_scale;
}

glm::mat4 Transform::get_matrix() const
{
	update_final_matrix();

	return m_final_mat;
}

glm::vec3 Transform::get_euler_angles() const
{
	return glm::eulerAngles(m_quat);
}

glm::vec3 Transform::get_forward() const
{
	glm::vec3 forward = glm::vec3(1.f, 0.f, 0.f);

	return glm::rotate(m_quat, forward);
}

void Transform::update_final_matrix() const
{
	if (is_changed)
	{
		const glm::mat4 scale = glm::scale(glm::toMat4(m_quat), m_scale);
		
		//const glm::mat4 pivot = glm::translate(scale, -m_pivot_point);
		
		m_final_mat = glm::translate(m_pos) * scale;
		
		//m_final_mat = translate * rot * pivot * scale;

		is_changed = false;
	}
}
