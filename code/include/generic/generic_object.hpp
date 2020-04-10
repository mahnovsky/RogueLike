#pragma once

#include "type_info.hpp"

class IGenericObject
{
public:
	virtual ~IGenericObject() = default;

	virtual const char* get_typename() const = 0;

	virtual size_t type_index() const = 0;

	virtual int type_name_space() const = 0;
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
int type_name_space() const override { return TypeInfo<type, ns>::name_space; } 