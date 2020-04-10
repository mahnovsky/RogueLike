#pragma once

#include <vector>
#include <cassert>

template <class T>
class Pool
{
public:
	static constexpr uint32_t DEAD_MARK = 0xDEADDEAD;
	Pool()
	{
		_inner_grow();
	}

	Pool(size_t buff_size)
		:_buffer_size(buff_size)
		,_buffers()
		,_free_objects()
	{
		_inner_grow();
	}

	~Pool()
	{
		_free_objects.clear();
		for(auto b : _buffers)
		{
			for (size_t i = 0; i < b.pos; ++i)
			{
				T* obj = reinterpret_cast<T*>(b.objects + (i * sizeof(T)));
				uint32_t* ptr = reinterpret_cast<uint32_t*>(obj);
				if ((*ptr) != DEAD_MARK)
				{
					obj->~T();
				}
			}
			::free(b.objects);
		}
	}

	template <class ... Args>
	T* alloc(Args ... args)
	{
		if(!_free_objects.empty())
		{
			auto obj = _free_objects.back();
			_free_objects.pop_back();

			return new (obj) T(args ...);
		}

		return _inner_allocate(args ...);
	}

	void free(T* obj)
	{
		assert(std::find(_free_objects.begin(), _free_objects.end(), obj) == _free_objects.end());

		obj->~T();
		uint32_t* ptr = reinterpret_cast<uint32_t*>(obj);
		(*ptr) = DEAD_MARK;
		_free_objects.push_back(obj);
	}

	size_t get_memory_size() const
	{
		const size_t total = _buffers.size() * _buffer_size * sizeof(T);
		
		return sizeof(Pool<T>) + total;
	}

	std::vector<T*> get_objects() const
	{
		std::vector<T*> res;
		for(auto& b : _buffers)
		{
			for (size_t i = 0; i < b.pos; ++i)
			{
				T* obj = reinterpret_cast<T*>(b.objects + (i * sizeof(T)));
				uint32_t* ptr = reinterpret_cast<uint32_t*>(obj);
				if ((*ptr) != DEAD_MARK)
				{
					res.push_back(obj);
				}
			}
		}

		return res;
	}
	template <class Func>
	void do_foreach(Func f)
	{
		for (auto& b : _buffers)
		{
			for (size_t i = 0; i < b.pos; ++i)
			{
				T* obj = reinterpret_cast<T*>(b.objects + (i * sizeof(T)));
				uint32_t* ptr = reinterpret_cast<uint32_t*>(obj);
				if ((*ptr) != DEAD_MARK)
				{
					f(obj);
				}
			}
		}
	}

private:
	struct Buffer
	{
		uint8_t* objects = nullptr;
		size_t pos = 0;

		Buffer(uint8_t* ptr, size_t p)
			:objects(ptr)
			,pos(p)
		{}
	};

	template <class ... Args>
	T* _inner_allocate(Args ... args)
	{
		Buffer& b = _buffers.back();
		if (b.pos < _buffer_size)
		{
			T* obj = reinterpret_cast<T*>(b.objects + (b.pos * sizeof(T)));
			++b.pos;
			
			return new (obj) T(args ...);
		}

		_inner_grow();

		return _inner_allocate(args ...);
	}

	void _inner_grow()
	{
		_buffers.emplace_back(reinterpret_cast<uint8_t*>(malloc(_buffer_size * sizeof(T))), 0);
	}

private:
	size_t _buffer_size = 64;
	std::vector<Buffer> _buffers;
	
	std::vector<T*> _free_objects;
};
