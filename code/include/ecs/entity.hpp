#pragma once

#include <vector>
#include <string>

#include "type_registration.hpp"

#include "generic/generic_object.hpp"
#include "ecs_manager.hpp"
#include "component.hpp"
#include "hash_to_index.hpp"

class Entity : public TGenericObject<Entity, IGenericObject, "Entity">
{
public:

	Entity(EntityComponentManager* mng);

	~Entity() override;

	template <class T>
	bool is_component_exist() const
	{
		return is_component_exist(TypeInfo<T, ComponentType>::type_index);
	}

	bool is_component_exist(size_t type_index) const;

	static uint32_t get_component_index(const Component* comp);

	void add_component(Component* comp);

	Component* get_component(uint32_t type_index);

	const Component* get_component(uint32_t type_index) const;

	EntityComponentManager* get_manager() const;

	std::vector<Component*> get_components() const;

	template <class T>
	T* get_component()
	{
		return dynamic_cast<T*>(get_component(TypeInfo<T, ComponentType>::type_index));
	}

	template <class T>
	const T* get_component() const
	{
		return dynamic_cast<const T*>(get_component(TypeInfo<T, ComponentType>::type_index));
	}

	template <class T, class ... Args>
	T* add_component(Args ... args)
	{
		T* comp = m_manager->create_component<T, Args ...>(this, args ...);

		add_component(dynamic_cast<Component*>(comp));

		return comp;
	}

	Entity* get_parent();

	void add_child(Entity* child);

	void remove_child(Entity* child);

	void send_component_event(Component* sender, ComponentEvent event_type);

private:
	static inline HashToIndex m_hash_to_index;
	EntityComponentManager* m_manager;
	uint64_t m_components_flag = 0;
	std::vector<Component*> m_components;


	Entity* m_parent;
	std::vector<Entity*> m_children;
};