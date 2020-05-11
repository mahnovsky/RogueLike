#include "entity.hpp"
#include "basic/debug.hpp"

Entity::Entity(EcsManager* mng): m_manager(mng)
{
}

Entity::~Entity()
{
	for (auto& child : m_children)
	{
		m_manager->free_entity(child);
	}
}

bool Entity::is_component_exist(size_t type_index) const
{
	uint64_t test_flag = 1;
	test_flag <<= type_index;

	return (test_flag & m_components_flag) > 0;
}

void Entity::add_component(IGenericObject* comp)
{
	int type_index = comp->type_index();
	ASSERT(type_index < sizeof(uint64_t));

	uint64_t test_flag = 1;
	test_flag <<= type_index;

	ASSERT((test_flag & m_components_flag) == 0);
	m_components_flag |= test_flag;

	m_components.push_back(comp);
}

IGenericObject* Entity::get_component(size_t type_index)
{
	if (is_component_exist(type_index))
	{
		auto it = std::find_if(m_components.begin(), m_components.end(), [type_index](IGenericObject* obj)
			{
				return type_index == obj->type_index();
			});

		if (it != m_components.end())
			return *it;;
	}

	return nullptr;
}

EcsManager* Entity::get_manager() const
{
	return m_manager;
}

std::vector<IGenericObject*> Entity::get_components() const
{
	return m_components;
}

Entity* Entity::get_parent()
{
	return m_parent;
}

void Entity::add_child(Entity* child)
{
	ASSERT(child != nullptr);
	auto it = std::find(m_children.begin(), m_children.end(), child);
	ASSERT(it == m_children.end());
	if (child->m_parent != nullptr)
	{
		child->m_parent->remove_child(child);
	}

	m_children.push_back(child);
	child->m_parent = this;
}

void Entity::remove_child(Entity* child)
{
	ASSERT(child != nullptr);
	ASSERT(child->m_parent == this);
	auto it = std::find(m_children.begin(), m_children.end(), child);
	if (it != m_children.end())
	{
		m_children.erase(it);
		child->m_parent = nullptr;
	}
}
