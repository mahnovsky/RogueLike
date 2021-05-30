#pragma once

#include "types.hpp"

using TypeIndex = uint64_t;

template <class T, int ns>
struct TypeInfo
{
	static const int name_space = ns;
	static const char* type_name;
	static const TypeIndex type_index;
};

#define INVALID_TYPE_INDEX 0

template <class T, int ns>
const char* TypeInfo<T, ns>::type_name = "Invalid";

template <class T, int ns>
const TypeIndex TypeInfo<T, ns>::type_index = INVALID_TYPE_INDEX;

#define REGISTRY_TYPE(type, index, name_space) \
class type; \
template <> \
const char* TypeInfo<type, name_space>::type_name = #type; \
template <> \
const TypeIndex TypeInfo<type, name_space>::type_index = index + 1; \

#define NS_REGISTRY_TYPE(ns, type, index, name_space) \
namespace ns { class type; } \
using full_type = ns::type; \
template <> \
const char* TypeInfo<full_type, name_space>::type_name = #type; \
template <> \
const TypeIndex TypeInfo<full_type, name_space>::type_index = index + 1; \
