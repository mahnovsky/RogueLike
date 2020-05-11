#pragma once

#include <vector>
#include <string>

#include "type_registration.hpp"

#include "generic/generic_object.hpp"
#include "ecs_manager.hpp"

class Entity : public IGenericObject
{
public:
	GENERIC_OBJECT_IMPL(Entity, NS_ENTITY_TYPE);

	Entity(EcsManager* mng);

	~Entity() override;

	template <class T>
	bool is_component_exist() const
	{
		return is_component_exist(TypeInfo<T, NS_COMPONENT_TYPE>::type_index);
	}

	bool is_component_exist(size_t type_index) const;

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

	Entity* get_parent();

	void add_child(Entity* child);

	void remove_child(Entity* child);

private:
	EcsManager* m_manager;
	uint64_t m_components_flag = 0;
	std::vector<IGenericObject*> m_components;

	Entity* m_parent;
	std::vector<Entity*> m_children;
};
