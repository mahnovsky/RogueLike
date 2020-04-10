#pragma once

template <class T, int ns>
struct TypeInfo
{
	static const int name_space = ns;
	static const char* type_name;
	static const size_t type_index;
};

#define INVALID_TYPE_INDEX 0

template <class T, int ns>
const char* TypeInfo<T, ns>::type_name = "Invalid";

template <class T, int ns>
const size_t TypeInfo<T, ns>::type_index = INVALID_TYPE_INDEX;

#define REGISTRY_TYPE(type, index, name_space) \
class type; \
template <> \
const char* TypeInfo<type, name_space>::type_name = #type; \
template <> \
const size_t TypeInfo<type, name_space>::type_index = index;
