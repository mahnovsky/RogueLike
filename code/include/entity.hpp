#pragma once

#include "defines.hpp"

using ComponentTypeID = basic::uint32;

class IComponent
{
public:
	static ComponentTypeID generate_type_id()
	{
		static ComponentTypeID counter;
		return counter++;
	}

	template <class T>
	static ComponentTypeID type_id()
	{
		static const ComponentTypeID id = IComponent::generate_type_id();

		return id;
	}

	virtual ComponentTypeID get_type_id() const = 0;
};

template <class T>
class Component
{
public:
	static const ComponentTypeID _type_id;

	ComponentTypeID get_type_id() const override
	{
		return _type_id;
	}
};

template <class T>
const ComponentTypeID Component<T>::_type_id = IComponent::type_id<T>();

class TransformComponent : public Component<TransformComponent>
{
public:
	glm::vec3 pos;
};

class Entity
{
public:
    Entity( );
    virtual ~Entity( );

    void init( World* world );

	struct ComponentHandle
	{

	};

private:
    World* m_world;

	basic::Vector<IComponent*> m_components;
};
