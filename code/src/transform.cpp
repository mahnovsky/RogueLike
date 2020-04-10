#include "transform.hpp"

#include "glm/ext.hpp"
#include "entity.hpp"
#include "octree.h"

Transform::Transform(Entity* ent)
	: Component(ent)
	, m_final_mat(1.f)
	, m_parent(nullptr)
	, m_pos()
	, m_euler_angles()
	, m_pivot_point()
	, m_quat()
	, m_scale(1.f)
{
}

Transform::~Transform()
{
	if (!m_children.is_empty())
	{
		for (auto child : m_children)
		{
			if (child->m_parent == nullptr)
				continue;
			ASSERT(child->m_parent == this);
			child->m_parent = nullptr;
		}
	}
	if (m_parent)
	{
		m_parent->remove(this);
	}
}

void Transform::set_parent(Transform* parent)
{
	ASSERT(parent);
	if (m_parent)
	{
		m_parent->remove(this);
	}
	m_parent = parent;

	update_final_matrix();
}

void Transform::add_child(Transform* child)
{
	m_children.push(child);

	child->set_parent(this);
}

void Transform::remove(Transform* child)
{
	ASSERT(child->m_parent == this);
	m_children.remove_by_value(child);

	child->m_parent = nullptr;
}

void
Transform::set_position(const glm::vec3& pos)
{
	m_pos = pos;
	if (m_entity)
	{
		auto bound = m_entity->get_component<OctreeObject>();
		if (bound)
		{
			bound->set_position({ pos.x, pos.y, pos.z });
		}
	}

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
	m_euler_angles = angles;
	m_quat = glm::quat(angles);

	is_changed = true;
}

void Transform::set_scale(const glm::vec3& scale)
{
	m_scale = scale;

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
	return m_euler_angles;
}

glm::vec3 Transform::get_forward() const
{
	glm::vec3 forward;
	forward.x = glm::sin(m_euler_angles.y);
	forward.y = -glm::tan(m_euler_angles.x);
	forward.z = glm::cos(m_euler_angles.y);

	return forward;
}

void Transform::update_final_matrix() const
{
	if (is_changed)
	{
		glm::mat4 const identity(1.f);

		const glm::mat4 pivot = glm::translate(identity, -m_pivot_point);
		const glm::mat4 translate = glm::translate(identity, m_pos);
		const glm::mat4 rot = glm::mat4_cast(m_quat);
		const glm::mat4 scale = glm::scale(identity, m_scale);

		m_final_mat = translate * rot * pivot * scale;

		is_changed = false;
	}

	if (m_parent)
	{
		m_final_mat = m_final_mat * m_parent->get_matrix();
	}

	/*for (Transform* child : m_children)
	{
		child->update_final_matrix();
	}*/
}
