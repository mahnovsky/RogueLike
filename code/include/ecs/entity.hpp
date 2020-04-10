#pragma once

#include <vector>
#include <string>

#include "ecs.hpp"

#include "generic/generic_object.hpp"
#include "ecs_manager.hpp"

class Entity : public IGenericObject
{
public:
	GENERIC_OBJECT_IMPL(Entity, NS_ENTITY_TYPE);

	Entity(EcsManager* mng);

	~Entity() override = default;

	void add_component(IGenericObject* comp);

	IGenericObject* get_component(size_t type_index);

	EcsManager* get_manager() const;

	std::vector<IGenericObject*> get_components() const;

	template <class T>
	T* get_component()
	{
		return dynamic_cast<T*>(get_component(TypeInfo<T, NS_COMPONENT_TYPE>::type_index));
	}

	template <class T, class ... Args>
	T* add_component(Args ... args)
	{
		T* comp = m_manager->create_component<T, Args ...>(this, args ...);

		add_component(comp);

		return comp;
	}


private:
	EcsManager* m_manager;

	std::vector<IGenericObject*> m_components;
	std::string m_name;
};
