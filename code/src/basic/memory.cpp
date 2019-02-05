#include "basic/memory.hpp"
#include "basic/util_functions.hpp"


#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <malloc.h>

#define MAX_BUFFER_LEN 1024

namespace basic
{

static mem_out_callback g_out_of_memory_callback;
static memory_size memory_usage;

template <class T>
static T* ptr_plus(void* ptr, size_t offset)
{
    return reinterpret_cast<T*>( ( (static_cast<char*>(ptr)) + offset ) );
}

template <class T>
static T* ptr_minus(void* ptr, size_t offset)
{
    return reinterpret_cast<T*>( ( (static_cast<char*>(ptr)) - offset ) );
}

static memory_size get_mem_size(void* ptr)
{
    const uint32 offset = sizeof(memory_size) + sizeof(ref_count);

    return *ptr_minus<memory_size>( ptr, offset );
}

static memory_size get_user_mem_size(void* ptr)
{
    const uint32 offset = sizeof(memory_size) + sizeof(ref_count);

    return (*ptr_minus<memory_size>(ptr, offset)) - offset;
}

static void* get_base_ptr(void* ptr)
{
    const uint32 offset = sizeof(memory_size) + sizeof(ref_count);

    return ptr_minus<void>( ptr, offset );
}

static void* internal_malloc(memory_size byte_count, memory_size& real_size)
{
    const uint32 mem_offset = sizeof( memory_size );
    const uint32 offset = mem_offset + sizeof( ref_count );

    real_size = byte_count + offset;

    void* res = malloc( real_size );
    if (res)
    {
        (*static_cast<memory_size*>(res)) = real_size;
        ref_count* refs = ptr_plus<ref_count>(res, mem_offset);
        (*refs) = 0;

        return ptr_plus<void>(res, offset);
    }

    return nullptr;
}

static void internal_free(void* ptr, size_t& free_bytes )
{
    free_bytes = get_mem_size( ptr );

    ASSERT( get_refs(ptr) == 0 );

    void* base_ptr = get_base_ptr(ptr);

    free( base_ptr );
}

void*
mem_allocate( memory_size byte_count )
{
    ASSERT_M( byte_count > 0, "Bad alloc size" );

    memory_size out_bytes = 0;
    void* res = internal_malloc( byte_count, out_bytes );

    if ( !res && g_out_of_memory_callback )
    {
        g_out_of_memory_callback( );
    }

    memory_usage += out_bytes;

    return res;
}

void
mem_free( void* ptr )
{
    size_t free_bytes = 0;

    internal_free( ptr, free_bytes );

    memory_usage -= free_bytes;
}

void*
mem_move(void* destination, const void* source, memory_size byte_count )
{
    ASSERT_M( destination != nullptr, "Destination is nullptr" );
    ASSERT_M( source != nullptr, "Source is nullptr" );
    ASSERT_M( byte_count > 0, "Bad alloc size" );

    return memmove( destination, source, byte_count );
}

void*
mem_copy( void* destination, const void* source, memory_size byte_count )
{
    ASSERT_M( destination != nullptr, "Destination is nullptr" );
    ASSERT_M( source != nullptr, "Source is nullptr" );
    ASSERT_M( byte_count > 0, "Bad alloc size" );

    return memcpy( destination, source, byte_count );
}

int
mem_cmp(const void* ptr1, const void* ptr2, memory_size byte_count )
{
    ASSERT( ptr1 != nullptr );
    ASSERT( ptr2 != nullptr );
    ASSERT( byte_count > 0 );

    return memcmp( ptr1, ptr2, byte_count );
}

void*
mem_realloc(void* ptr, memory_size byte_count )
{
    ASSERT( byte_count > 0 );

    void* res = ptr;

    if ( ptr )
    {
        const memory_size mem_size = get_user_mem_size(ptr);

        if (byte_count > mem_size)
        {
            res = mem_allocate(byte_count);

            mem_copy(res, ptr, mem_size);

            mem_free(ptr);
        }
    }
    else
    {
        res = mem_allocate(byte_count);
    }

    return res;
}

void
mem_set_out_of_memory( mem_out_callback callback )
{
    ASSERT( callback != nullptr );

    g_out_of_memory_callback = callback;
}

size_t get_memory_usage()
{
    return memory_usage;
}

ref_count increment_ref(void *ptr)
{
    ref_count* refs = ptr_minus<ref_count>(ptr, sizeof(ref_count));

    return ++(*refs);
}

ref_count decrement_ref(void *ptr)
{
    ref_count* refs = ptr_minus<ref_count>(ptr, sizeof(ref_count));

    return --(*refs);
}

ref_count get_refs(void *ptr)
{
    ref_count* refs = ptr_minus<ref_count>(ptr, sizeof(ref_count));

    return (*refs);
}

}

void*
operator new( std::size_t n )
{
    return basic::mem_allocate( static_cast<basic::memory_size>(n) );
}

void
operator delete( void* p )
{
    basic::mem_free( p );
}

void*
operator new[]( std::size_t s )
{
    return basic::mem_allocate( static_cast<basic::memory_size>(s) );
}

void
operator delete[]( void* p )
{
    basic::mem_free( p );
}
