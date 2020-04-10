#include "entity.hpp"

Entity::Entity(EcsManager* mng): m_manager(mng)
{
}

void Entity::add_component(IGenericObject* comp)
{
	m_components.push_back(comp);
}

IGenericObject* Entity::get_component(size_t type_index)
{
	auto it = std::find_if(m_components.begin(), m_components.end(), [type_index](IGenericObject* obj)
	{
		return type_index == obj->type_index();
	});
	if (it != m_components.end())
		return *it;;

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
