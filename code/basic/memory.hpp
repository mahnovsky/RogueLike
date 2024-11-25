#pragma once

#include "types.hpp"
#include <source_location>
#include <vector>
#include <ranges>
#include <cassert>
#include <unordered_map>
#include <tuple>
#include <memory>

#ifdef _DEBUG

//#define mem_alloc( bytes ) _checked_mem_alloc( bytes, __FILE__, __LINE__ )
//#define mem_realloc( ptr, bytes ) _checked_mem_realloc( ptr, bytes, __FILE__, __LINE__ )
//#define mem_free _mem_free

#elif 0
#include <stdio.h>

#define mem_alloc wrapper_malloc
#define mem_realloc wrapper_realloc
#define mem_free wrapper_free

#else

#endif

#define MEMORY_ALLOCATION_CHECK 1

namespace basic
{
	using memory_size = size_t;
	using ref_count = int32;
	using mem_out_callback = void (*)();

	void* mem_alloc(memory_size bytes, const char* const file = __FILE__, int line = __LINE__);

	void mem_free(void* ptr);

	void* mem_realloc(void* ptr, memory_size bytes, const char* const file = __FILE__, int line = __LINE__);

	void* _checked_mem_alloc(memory_size bytes, const char* const file, int line);

	void* _checked_mem_realloc(void* ptr, memory_size bytes, const char* file, int line);

	void* mem_move(void* destination, const void* source, memory_size bytes);

	void* mem_copy(void* destination, const void* source, memory_size bytes);

	int mem_cmp(const void* ptr1, const void* ptr2, memory_size bytes);

	void mem_set_out_of_memory(mem_out_callback callback);

	size_t get_memory_usage();

	void* wrapper_malloc(size_t size);

	void* wrapper_realloc(void* ptr, size_t size);

	void wrapper_free(void* ptr);

	template < class T >
	uint32 size_of()
	{
		return sizeof(T);
	}

	template < class T, class T1, class... Args >
	uint32 size_of()
	{
		return sizeof(T) + size_of< T1, Args... >();
	}

	template < typename... Args >
	void* alloc_objects(const char* file, int line)
	{
		uint32 size = size_of< Args... >();

		return _checked_mem_alloc(size, file, line);
	}

	template < typename T, typename... Args >
	T* init_object(void* ptr, uint32& offset, Args... args)
	{
		uint32 off = offset;
		offset += sizeof(T);
		char* cptr = static_cast<char*>(ptr);
		return new (cptr + off) T(args...);
	}

	template < typename T >
	void delete_obj(T* ptr)
	{
		if (ptr)
		{
			ptr->~T();
			mem_free(ptr);
		}
	}

	namespace defer
	{
		template <class F, class Tuple, bool Enough, int Total, int... N>
		struct ArgsUnpack
		{
			auto static call_impl(F f, Tuple&& t)
			{
				return ArgsUnpack<F, Tuple, Total == sizeof...(N) + 1, Total, N..., sizeof...(N)>::call_impl(f, t);
			}
		};

		template <class F, class Tuple, int Total, int... N>
		struct ArgsUnpack<F, Tuple, true, Total, N...>
		{
			auto static call_impl(F f, Tuple&& t)
			{
				return f(std::get<N>(std::forward<Tuple>(t))...);
			}
		};

		template <class F, class Tuple, int... N>
		struct ArgsUnpack<F, Tuple, false, 0, N...>
		{
			auto static call_impl(F f, Tuple&& t)
			{
				return f();
			}
		};

		template <class F, class Tuple>
		auto static call(F f, Tuple&& t)
		{
			using type = typename std::decay<Tuple>::type;

			return ArgsUnpack <F, Tuple, 1 == std::tuple_size<type>::value, std::tuple_size<type>::value, 0>::call_impl(f, t);
		}
	}

	struct Memory
	{
		template <class T, class ... Args>
		struct ObjectWrapper
		{
			ObjectWrapper() = delete;
			ObjectWrapper(const ObjectWrapper&) = delete;
			ObjectWrapper(ObjectWrapper&&) = default;
			ObjectWrapper(Memory* mem, std::tuple<Args...>&& args)
				:_args(args)
				,_memory(mem)
			{
			}

			T* Get(std::source_location location = std::source_location::current())
			{
				T* object = nullptr;
				if constexpr (std::tuple_size<std::tuple<Args...>>::value > 0)
				{
					object = defer::call([](Args ... args) -> T* { return new T(args ...); }, _args);
				}
				else
				{
					object = new T();
				}

				_memory->Registry(object, location);
				
				return object;
			}

			std::shared_ptr<T> GetShared(std::source_location location = std::source_location::current())
			{
				std::shared_ptr<T> shared;
				if constexpr (std::tuple_size<std::tuple<Args...>>::value > 1)
				{
					shared = defer::call([](Args ... args) -> T* { return std::make_shared<T>(args ...); }, _args);
				}
				else
				{
					shared = std::make_shared<T>();
				}
				_memory->Registry(shared.get(), location);
				
				return shared;
			}

			friend class Memory;
		private:
			std::tuple<Args ...> _args;
			mutable Memory* _memory;
		};

		template<class T, class ... Args>
		static ObjectWrapper<T, Args ...>&& New(Args&& ... args)
		{
			return ObjectWrapper<T, Args ...>(&_instance, std::tuple<Args...>(args ...));
		}

		template<class T>
		static void Delete(T* object)
		{
			if (_instance._memCheck)
			{
				auto pos = _instance._objects.find(object);

				assert(pos != _instance._objects.end());
				if (pos != _instance._objects.end() && object == pos->second.ptr)
				{
					delete object;
					_instance._objects.erase(pos);
				}
			}
		}

		static void PrintMemStats()
		{
			_instance.PrintStats();
		}

		template<class T, class ... Args>
		void Registry(T* ptr, std::source_location location)
		{
			if (_instance._memCheck)
			{
				_objects.emplace(std::make_pair(ptr, MemObject{ ptr, sizeof(T), location }));
			}
		}

	private:
		void PrintStats();

		struct MemObject
		{
			void* ptr;
			size_t size;
			std::source_location location;
		};

		std::unordered_map<void*, MemObject> _objects;
		bool _memCheck = true;

		static Memory _instance;
	};
}

#define NEW_OBJ( TYPE, ... ) basic::Memory::New<TYPE>(__VA_ARGS__).Get()
#define DELETE_OBJ( ptr ) basic::Memory::Delete(ptr)
