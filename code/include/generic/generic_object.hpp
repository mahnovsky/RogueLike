#pragma once

#include <string>

#include "type_info.hpp"
#include "memory.hpp"
#include "debug.hpp"

class IGenericObject
{
public:
	virtual ~IGenericObject() = default;

	virtual const char* get_typename() const = 0;

	virtual size_t type_index() const = 0;

	virtual size_t get_type_hash() const = 0;
};

template <class T, class N, class P = IGenericObject, StringLiteral lit = "">
class TGenericObject : public P
{
public:
	template <typename ... Args>
	TGenericObject(Args ... args)
		:P(args ...)
	{}

	const char* get_typename() const override { 
		return lit.value;
	}

	size_t type_index() const override {
		return TypeInfo<T, N>::type_index;
	}

	size_t get_type_hash() const {
		return lit.get_hash();
	}

	static const char* get_type_name() { return lit.value; }
	static size_t GetTypeHash() { return lit.get_hash(); }
};

template <class T, class N>
T* fast_cast(IGenericObject* obj)
{
	ASSERT( (obj->type_index() == static_cast<size_t>(TypeInfo<T, N>::type_index)) );
	
	return reinterpret_cast<T*>(obj);
}


#define GENERIC_OBJECT_IMPL(type, ns) \
const char* get_typename() const override \
{ return TypeInfo<type, ns>::type_name; } \
size_t type_index() const override \
{ return TypeInfo<type, ns>::type_index; } \
size_t get_type_hash() const { return 0; }