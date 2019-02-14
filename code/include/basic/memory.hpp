#pragma once

#include "types.hpp"

namespace basic
{

using memory_size = uint32;
using ref_count = int32;
using mem_out_callback = void ( * )( );

void* _mem_alloc( memory_size bytes );

void* _mem_realloc( void* ptr, memory_size bytes );

void* _checked_mem_alloc( memory_size bytes, const char* file, int line );

void* _checked_mem_realloc( void* ptr, memory_size bytes, const char* file, int line );

void mem_free( void* mem );

void* mem_move( void* destination, const void* source, memory_size bytes );

void* mem_copy(void* destination, const void* source, memory_size bytes );

int mem_cmp( const void* ptr1, const void* ptr2, memory_size bytes );

void mem_set_out_of_memory( mem_out_callback callback );

size_t get_memory_usage();

ref_count increment_ref(void* ptr);

ref_count decrement_ref(void* ptr);

ref_count get_refs(void* ptr);

}

void* operator new( std::size_t n );
void operator delete( void* p ) noexcept;

void* operator new[]( std::size_t s );
void operator delete[]( void* p ) noexcept;

#ifdef _DEBUG

#define mem_alloc( bytes ) _checked_mem_alloc( bytes, __FILE__, __LINE__ )
#define mem_realloc( ptr, bytes ) _checked_mem_realloc( ptr, bytes, __FILE__, __LINE__ )

#else

#define mem_alloc _mem_alloc
#define mem_realloc _mem_realloc

#endif
