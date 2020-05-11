#pragma once

#include <string>

#include "type_info.hpp"
#include "basic/memory.hpp"

class IGenericObjectListener
{
public:
	virtual void on_object_unref(class IGenericObject* ) = 0;
};

class IGenericObject
{
public:
	virtual ~IGenericObject() = default;

	virtual const char* get_typename() const = 0;

	virtual size_t type_index() const = 0;

	virtual size_t type_name_space() const = 0;

	virtual void on_destruct() { DELETE_OBJ(this); }

	const std::string& get_name() const
	{
		return m_name;
	}

	void set_name(const std::string& name)
	{
		m_name = name;
	}

	void set_listener(IGenericObjectListener* listener)
	{
		m_listener = listener;
	}

	void ref() { ++m_refs; }

	void deref() 
	{
		--m_refs; 
		if (m_listener)
		{
			m_listener->on_object_unref(this);
		}
	}

	int32_t get_refs() const { return m_refs; }
	
private:
	IGenericObjectListener* m_listener = nullptr;
	int32_t m_refs = 0;
	std::string m_name;
};

template <class T, size_t ns>
T* fast_cast(IGenericObject* obj)
{
	if(obj->type_index() == TypeInfo<T, ns>::type_index)
	{
		return reinterpret_cast<T*>(obj);
	}
	return nullptr;
}


#define GENERIC_OBJECT_IMPL(type, ns) \
const char* get_typename() const override \
{ return TypeInfo<type, ns>::type_name; } \
size_t type_index() const override \
{ return TypeInfo<type, ns>::type_index; } \
size_t type_name_space() const override { return TypeInfo<type, ns>::name_space; } 