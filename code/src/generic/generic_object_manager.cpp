#include "generic_object_manager.hpp"
#include <algorithm>

void GenericObjectManager::add_object(IGenericObject* object)
{
	auto it = std::find(m_objects.begin(), m_objects.end(), object);
	if (it != m_objects.end())
	{
		return;
	}
	object->set_listener(this);
	m_objects.push_back(object);
	const size_t ns = object->type_name_space();
	if (m_name_space_objects.size() <= ns)
	{
		m_name_space_objects.resize(ns + 1);
		m_name_space_objects[ns].push_back(object);
	}
}

void GenericObjectManager::remove_object(IGenericObject* object)
{
	auto it = std::find(m_objects.begin(), m_objects.end(), object);
	if (it != m_objects.end())
	{
		m_objects.erase(it);

		const size_t ns = object->type_name_space();
		if (m_name_space_objects.size() > ns)
		{
			auto ns_it = std::find(m_name_space_objects[ns].begin(), m_name_space_objects[ns].end(), object);
			if (ns_it != m_name_space_objects[ns].end())
			{
				m_name_space_objects[ns].erase(ns_it);
			}
		}
	}
}

IGenericObject* GenericObjectManager::find(std::string_view name, size_t name_space)
{
	if (name_space < m_name_space_objects.size())
	{
		return find(m_name_space_objects[name_space], name);
	}
	return nullptr;
}

IGenericObject* GenericObjectManager::find(std::string_view name)
{
	return find(m_objects, name);
}

IGenericObject* GenericObjectManager::find(GenericObjectVector& cont, std::string_view name)
{
	auto it = std::find_if(cont.begin(), cont.end(), [name](IGenericObject* object)
		{
			return object->get_name() == name;
		});

	if (it != cont.end())
	{
		return *it;
	}

	return nullptr;
}

void GenericObjectManager::on_object_unref(IGenericObject* obj)
{
	remove_object(obj);
}
