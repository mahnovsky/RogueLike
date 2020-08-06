#pragma once

#include "basic/types.hpp"

template <class T, int ns>
struct TypeInfo
{
	static const int name_space = ns;
	static const char* type_name;
	static const uint32_t type_index;
	static const uint32_t type_uid;
};

#define INVALID_TYPE_INDEX 0

template <class T, int ns>
const char* TypeInfo<T, ns>::type_name = "Invalid";

template <class T, int ns>
const uint32_t TypeInfo<T, ns>::type_index = INVALID_TYPE_INDEX;

template <class T, int ns>
const uint32_t TypeInfo<T, ns>::type_uid = INVALID_TYPE_INDEX;

#define REGISTRY_TYPE(type, index, name_space) \
class type; \
template <> \
const char* TypeInfo<type, name_space>::type_name = #type; \
template <> \
const uint32_t TypeInfo<type, name_space>::type_index = index; \
template <> \
const uint32_t TypeInfo<type, name_space>::type_uid = ((uint32_t)1 << index) | name_space;

#define NS_REGISTRY_TYPE(ns, type, index, name_space) \
namespace ns { class type; } \
using full_type = ns::type; \
template <> \
const char* TypeInfo<full_type, name_space>::type_name = #type; \
template <> \
const uint32_t TypeInfo<full_type, name_space>::type_index = index; \
template <> \
const uint32_t TypeInfo<full_type, name_space>::type_uid = ((uint32_t)1 << index) | name_space;
