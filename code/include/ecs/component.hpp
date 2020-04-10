#pragma once

#include "generic/generic_object.hpp"

class Component : public IGenericObject
{
public:
	Component(Entity* ent):m_entity(ent){}
	~Component() override = default;

	Entity* get_entity() const
	{
		return m_entity;
	}

protected:
	Entity* m_entity;
};
