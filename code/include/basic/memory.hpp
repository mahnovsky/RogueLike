#pragma once

#include "types.hpp"

namespace basic
{

using memory_size = uint64;
using ref_count = int32;
using mem_out_callback = void ( * )( );

void* mem_allocate( memory_size byte_count );

void mem_free( void* mem );

void* mem_move( void* destination, const void* source, memory_size byte_count );

void* mem_realloc( void* ptr, memory_size byte_count );

void* mem_copy(void* destination, const void* source, memory_size byte_count );

int mem_cmp( const void* ptr1, const void* ptr2, memory_size byte_count );

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
