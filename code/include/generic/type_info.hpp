#pragma once

#include "types.hpp"
#include "crc32.hpp"
#include <algorithm>

template <size_t N>
class StringLiteral
{
public:
	constexpr StringLiteral(const char(&str)[N])
	{
		std::copy_n(str, N, value);
	}

	constexpr size_t get_hash() const
	{
		return COMPILE_TIME_CRC32_STR(value);
	}

	char value[N];
};

using TypeIndex = uint64_t;


template <class T, class N>
struct TypeInfo
{
	static const char* type_name;
	static const N type_index;
};

constexpr auto INVALID_TYPE_INDEX = -1;

template <class T, class N>
const char* TypeInfo<T, N>::type_name = "Invalid";

template <class T, class N>
const N TypeInfo<T, N>::type_index = static_cast<N>(INVALID_TYPE_INDEX);

#define REGISTRY_TYPE(type, index, name_space) \
class type; \
template <> \
const char* TypeInfo<type, name_space>::type_name = #type; \
template <> \
const name_space TypeInfo<type, name_space>::type_index = index; \

