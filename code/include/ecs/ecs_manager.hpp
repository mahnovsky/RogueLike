#pragma once

#include <memory>
#include <algorithm>

#include "pool.hpp"
#include "type_registration.hpp"
#include "generic/generic_object.hpp"

class IGenericObject;
class Entity;


class IContainer
{
public:
	virtual ~IContainer() = default;

	virtual void free(IGenericObject*) = 0;
};

template <class T>
class Container : public IContainer
{
public:
	~Container() override
	{
		auto&& objects = pool.get_objects();
		for (auto obj : objects)
		{
			pool.free(obj);
		}
	}

	void free(IGenericObject* ent) override
	{
		T* obj = dynamic_cast<T*>(ent);
		pool.free(obj);
	}

	template <class Func>
	void do_foreach(Func f)
	{
		pool.do_foreach<Func>(f);
	}

	Pool<T> pool;
};

class EntityComponentManager
{
public:
	EntityComponentManager()
	{}
	~EntityComponentManager()
	{
		auto cont = std::move(m_systems);
		for (auto sys : cont)
		{
			delete sys;
		}
	}

	template <class T, class ... Args>
	T* create_entity(Args ... args)
	{
		const auto index = TypeInfo<T, NS_ENTITY_TYPE>::type_index;

		if(m_ent_containers.size() < (index + 1))
		{
			m_ent_containers.resize(index + 1);
		}

		if(!m_ent_containers[index])
		{
			m_ent_containers[index].reset(new Container<T>{});
		}

		auto cont = dynamic_cast<Container<T>*>(m_ent_containers[index].get());

		T* ent = cont->pool.alloc(this, args ...);

		return ent;
	}

	void free_entity(IGenericObject* ent)
	{
		const auto index = ent->type_index();

		assert(index < m_ent_containers.size());

		m_ent_containers[index]->free(ent);
	}

	template <class T, class ... Args>
	T* create_component(Entity* ent, Args ... args)
	{
		const auto index = TypeInfo<T, NS_COMPONENT_TYPE>::type_index;

		if (m_comp_containers.size() < (index + 1))
		{
			m_comp_containers.resize(index + 1);
		}

		if (!m_comp_containers[index])
		{
			m_comp_containers[index].reset(new Container<T>{});
		}

		auto cont = dynamic_cast<Container<T>*>(m_comp_containers[index].get());

		T* comp = cont->pool.alloc(ent, args ...);

		return comp;
	}

	template <class T>
	std::vector<T*> get_components() const
	{
		const size_t index = TypeInfo<T, NS_COMPONENT_TYPE>::type_index;

		auto cont = dynamic_cast<Container<T>*>(m_comp_containers[index].get());

		return cont->pool.get_objects();
	}

	template <class T>
	std::vector<T*> get_entities() const
	{
		const size_t index = TypeInfo<T, NS_ENTITY_TYPE>::type_index;

		auto cont = dynamic_cast<Container<T>*>(m_ent_containers[index].get());

		return cont->pool.get_objects();
	}

	template <class T, class Func>
	void foreach_component( Func f)
	{
		const size_t index = TypeInfo<T, NS_COMPONENT_TYPE>::type_index;

		auto cont = dynamic_cast<Container<T>*>(m_comp_containers[index].get());

		cont->do_foreach(f);
	}

	template <class T, class ... Args>
	T* add_system(Args ... args)
	{
		auto it = std::find_if(m_systems.begin(), m_systems.end(), [](IGenericObject* obj)
			{
				return obj->type_index() == TypeInfo<T, NS_SYSTEM_TYPE>::type_index;
			});
		if (it != m_systems.end())
		{
			return dynamic_cast<T*>(*it);
		}

		auto system = new T(this, args ...);

		m_systems.emplace_back(system);

		return system;
	}

	template <class T>
	T* get_system() const
	{
		auto it = std::find_if(m_systems.begin(), m_systems.end(), [](IGenericObject* obj)
			{
				return TypeInfo<T, NS_SYSTEM_TYPE>::type_index == obj->type_index();
			});

		if(it != m_systems.end())
		{
			return dynamic_cast<T*>(*it);
		}

		return nullptr;
	}

private:
	std::vector<std::unique_ptr<IContainer>> m_ent_containers;
	std::vector<std::unique_ptr<IContainer>> m_comp_containers;

	std::vector<IGenericObject*> m_systems;
};