#pragma once

#include <vector>
#include <string_view>
#include <algorithm>
#include "generic_object.hpp"


class GenericObjectManager : public IGenericObjectListener
{
	using GenericObjectVector = std::vector<IGenericObject*>;
public:
	GenericObjectManager() = default;

	void add_object(IGenericObject* object);

	void remove_object(IGenericObject* object);

	IGenericObject* find(std::string_view name, size_t name_space);

	IGenericObject* find(std::string_view name);

	template <class T, int NS>
	T* find_by_type()
	{
		auto cont = m_name_space_objects[NS];

		auto it = std::find_if(cont.begin(), cont.end(), [](IGenericObject* obj)
			{
				return obj->type_index() == TypeInfo<T, NS>::type_index;
			});
		
		if(it != cont.end())
			return dynamic_cast<T*>(*it);

		return nullptr;
	}

private:
	IGenericObject* find(GenericObjectVector& cont, std::string_view name);

	void on_object_unref( IGenericObject* obj ) override;

private:
	GenericObjectVector m_objects;
	std::vector<GenericObjectVector> m_name_space_objects;
};