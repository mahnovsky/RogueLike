#include "entity.hpp"
#include "debug.hpp"
#include "component.hpp"

Entity::Entity(EntityComponentManager* mng)
	: m_manager(mng)
	, m_parent(nullptr)
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

uint32_t Entity::get_component_index(const Component* comp)
{
	return m_hash_to_index.get_or_create_index(comp->get_meta_info()->hash);
}

void Entity::add_component(Component* comp)
{
	const uint32_t type_index = get_component_index(comp);
	uint64_t test_flag = 1;
	test_flag <<= type_index;

	ASSERT((test_flag & m_components_flag) == 0);
	m_components_flag |= test_flag;

	m_components.push_back(comp);
}

Component* Entity::get_component(uint32_t type_index)
{
	if (is_component_exist(type_index))
	{
		auto it = std::find_if(m_components.begin(), m_components.end(), [type_index, this](Component* obj)
			{
				return type_index == get_component_index(obj);
			});

		if (it != m_components.end())
			return *it;;
	}

	return nullptr;
}

const Component* Entity::get_component(uint32_t type_index) const
{
	if (is_component_exist(type_index))
	{
		const auto it = std::find_if(m_components.begin(), m_components.end(), 
			[type_index, this](const Component* obj)
			{
				return type_index == get_component_index(obj);
			});

		if (it != m_components.end())
			return *it;
	}

	return nullptr;
}

EntityComponentManager* Entity::get_manager() const
{
	return m_manager;
}

std::vector<Component*> Entity::get_components() const
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

void Entity::send_component_event(Component* sender, ComponentEvent event_type)
{
	const TypeIndex type_index = get_component_index(sender);

	for (const auto& component : m_components)
	{
		if (component->is_listen_component(type_index))
		{
			component->on_event(sender, event_type);
		}
	}
}
