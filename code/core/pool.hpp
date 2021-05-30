#pragma once

#include <vector>
#include <cassert>
#include "memory.hpp"

template <class T>
class Pool
{
public:
	static_assert(sizeof(T) > 32, "Size of object should be >= 32 bytes");
	static constexpr uint32_t DEAD_MARK = 0xDEADDEAD;

	Pool()
		: _max_node_elements(64)
		, _first(new_node())
		, _last(_first)
	{
	}

	Pool(size_t buff_size)
		:_max_node_elements(buff_size)
		,_first(new_node())
		,_last(_first)
	{		
	}

	~Pool()
	{
		free_all();
		PoolNode* node = _first;
		while (node)
		{
			PoolNode* next_node = node->next;
			basic::mem_free(node);
			node = next_node;
		}
	}

	void free_all()
	{
		auto&& objects = get_objects();
		for (auto obj : objects)
		{
			free(obj);
		}
	}

	T* get_prev_free(void* obj)
	{
		uint32_t* mask = static_cast<uint32_t*>(obj);
		assert((*mask) == DEAD_MARK);
		obj = mask + 1;
		return *static_cast<T**>(obj);
	}

	template <class ... Args>
	T* alloc(Args ... args)
	{
		if(_last_free)
		{
			auto res = _last_free;
			_last_free = get_prev_free(_last_free);

			return new (res) T(args ...);
		}

		return _inner_allocate(args ...);
	}

	void free(T* obj)
	{
		obj->~T();
		uint32_t* ptr = reinterpret_cast<uint32_t*>(obj);
		(*ptr) = DEAD_MARK;
		
		T** prev_free = static_cast<T**>(static_cast<void*>(ptr + 1));
		(*prev_free) = _last_free;
		_last_free = obj;
	}

	size_t get_memory_size() const
	{
		const size_t total = _nodes * _max_node_elements * sizeof(T);
		
		return sizeof(Pool<T>) + total;
	}

	std::vector<T*> get_objects() const
	{
		std::vector<T*> res;
		PoolNode* node = _first;
		
		while (node)
		{
			for (uint32_t i = 0; i < node->pos; ++i)
			{
				T* obj = reinterpret_cast<T*>(node->objects) + i;
				uint32_t* ptr = reinterpret_cast<uint32_t*>(obj);
				if ((*ptr) != DEAD_MARK)
				{
					res.push_back(obj);
				}
			}
			node = node->next;
		}

		return res;
	}

	template <class Func>
	void do_foreach(Func f)
	{
		PoolNode* node = _first;
		while (node)
		{
			for (uint32_t i = 0; i < node->pos; ++i)
			{
				T* obj = reinterpret_cast<T*>(node->objects) + i;
				uint32_t* ptr = reinterpret_cast<uint32_t*>(obj);
				if ((*ptr) != DEAD_MARK)
				{
					f(obj);
				}
			}
			node = node->next;
		}
	}

private:
	struct PoolNode
	{
		PoolNode* next;
		uint8_t* objects;
		size_t pos;
	};

	PoolNode* new_node()
	{
		const size_t mem_size = sizeof(PoolNode) + sizeof(T) * _max_node_elements;

		PoolNode* node = static_cast<PoolNode*>(basic::mem_alloc(mem_size));

		memset(node, 0, sizeof(PoolNode));

		node->objects = reinterpret_cast<uint8_t*>(node) + sizeof(PoolNode);

		++_nodes;

		return node;
	}

	template <class ... Args>
	T* _inner_allocate(Args ... args)
	{
		PoolNode* node = _last;

		if (node->pos < _max_node_elements)
		{
			T* obj = reinterpret_cast<T*>(node->objects + (node->pos * sizeof(T)));
			++node->pos;

			return new (obj) T(args ...);
		}

		_last->next = new_node();
		_last = _last->next;

		return _inner_allocate(args ...);
	}

private:
	const size_t _max_node_elements = 64;
	uint32_t _nodes = 0;
	PoolNode* _first = nullptr;
	PoolNode* _last = nullptr;
	T* _last_free = nullptr;
};
