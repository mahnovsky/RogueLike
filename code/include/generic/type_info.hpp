#pragma once

#include "types.hpp"
#include "crc32.hpp"
#include <algorithm>

template <uint32_t N>
class StringLiteral
{
public:
	constexpr StringLiteral(const char(&str)[N])
	{
		std::copy_n(str, N, value);
	}

	constexpr std::string_view get_name() const
	{
		return value;
	}

	constexpr uint32_t get_hash() const
	{
		return COMPILE_TIME_CRC32_STR(value);
	}

	char value[N];
};

template <class T, class N>
struct TypeInfo
{
	static const char* type_name;
	static const N type_index;
	static const uint32_t type_hash;
};

constexpr auto INVALID_TYPE_INDEX = -1;

template <class T, class N>
const char* TypeInfo<T, N>::type_name = "Invalid";

template <class T, class N>
const N TypeInfo<T, N>::type_index = static_cast<N>(INVALID_TYPE_INDEX);

template<class T, class N>
const uint32_t TypeInfo<T, N>::type_hash = 0;

#define CONCAT(A, B, C) A##B##C
#define TO_STR(A) #A

#define REGISTRY_TYPE(type, index, group) \
class type; \
	template <> \
const char* TypeInfo<type, group>::type_name = #type;            \
	template <> \
const group TypeInfo<type, group>::type_index = index; \
	template<>  \
const uint32_t TypeInfo<type, group>::type_hash = COMPILE_TIME_CRC32_STR(#type);

#define REGISTRY_TYPE_NS(ns, type, index, group) \
	namespace ns { class type; } \
	template<> \
	const char* TypeInfo<ns::type, group>::type_name = #type; \
	template<> \
	const group TypeInfo<ns::type, group>::type_index = index; \
	template<> \
	const uint32_t TypeInfo<ns::type, group>::type_hash = COMPILE_TIME_CRC32_STR(TO_STR(ns::type));
