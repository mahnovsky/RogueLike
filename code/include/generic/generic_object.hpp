#pragma once

#include <string>

#include "type_info.hpp"
#include "memory.hpp"
#include "debug.hpp"

using TypeIndex = uint64_t;

struct ObjectMetaInfo
{
	std::string_view name;
	uint32_t hash;

	constexpr ObjectMetaInfo(std::string_view name, uint32_t hash)
		:name(name)
		,hash(hash)
	{}
};

class IGenericObject
{
public:
	virtual ~IGenericObject() = default;

	virtual const ObjectMetaInfo* get_meta_info() const = 0;
};

template <class T, class P = IGenericObject, StringLiteral lit = "">
class TGenericObject : public P
{
public:
	using Super = TGenericObject<T, P, lit>;
	static constexpr ObjectMetaInfo CLASS_META_INFO { lit.get_name(), lit.get_hash() };

	template <typename ... Args>
	TGenericObject(Args ... args)
		:P(args ...)
	{}

	const ObjectMetaInfo* get_meta_info() const override { return &CLASS_META_INFO; }
};

template <class T, class N>
T* fast_cast(IGenericObject* obj)
{
	ASSERT(obj->get_meta_info()->hash == T::CLASS_META_INFO.hash );
	
	return static_cast<T*>(static_cast<void*>(obj));
}
