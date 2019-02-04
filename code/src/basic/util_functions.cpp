#include "basic/util_functions.hpp"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <malloc.h>

#ifdef __WINDOWS__
#include <Windows.h>
#endif

#define MAX_BUFFER_LEN 1024

namespace basic
{

mem_out_callback g_out_of_memory_callback;
size_t total_memory_usage = 0;

void
assert_func( int line, const char* file, const char* function, const char* message )
{
    puts( "Assertion failed!" );
    printf( "\t%s : %d\n", file, line );
    printf( "\tfunction: %s\n", function );
    printf( "\tmessage: %s\n", message );
    abort( );
}

size_t
str_length( const char* cstring, size_t max_len )
{
    ASSERT_M( cstring != nullptr, "cstring is nullptr" );
    ASSERT_M( max_len > 0, "max_len must be > 0" );

    return strnlen( cstring, max_len );
}

char*
str_copy( const char* cstr, size_t max_len )
{
    size_t size = str_length( cstr, max_len );

    char* res = static_cast< char* >( mem_allocate( size ) );
    mem_copy( res, cstr, size );

    if ( size == max_len )
    {
        res[ size ] = 0;
    }

    return res;
}

void*
mem_copy( void* destination, const void* source, size_t byte_count )
{
    ASSERT_M( destination != nullptr, "Destination is nullptr" );
    ASSERT_M( source != nullptr, "Source is nullptr" );
    ASSERT_M( byte_count > 0, "Bad alloc size" );

    return memcpy( destination, source, byte_count );
}

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

static void* internal_malloc(size_t byte_count, size_t& real_size)
{
    const uint32 mem_offset = sizeof( memory_size );
    const uint32 offset = mem_offset + sizeof( ref_count );

    real_size = byte_count + offset;

    void* res = malloc( real_size );
	if (res)
	{
		(*(memory_size*)res) = real_size;
		ref_count* refs = ptr_plus<ref_count>(res, mem_offset);
		(*refs) = 0;

		return ptr_plus<void>(res, offset);
	}

	return nullptr;
}

static void internal_free(void* ptr, size_t& free_bytes )
{
    free_bytes = get_mem_size( ptr );

	ref_count refs = get_refs(ptr);
    ASSERT( refs == 0 );

	void* base_ptr = get_base_ptr(ptr);

    free( base_ptr );
}

void*
mem_allocate( uint32 byte_count )
{
    ASSERT_M( byte_count > 0, "Bad alloc size" );

    size_t out_bytes = 0;
    void* res = internal_malloc( byte_count, out_bytes );

    if ( !res && g_out_of_memory_callback )
    {
        g_out_of_memory_callback( );
    }

    total_memory_usage += out_bytes;

    return res;
}

void
mem_free( void* ptr )
{
    size_t free_bytes = 0;

    internal_free( ptr, free_bytes );

    total_memory_usage -= free_bytes;
}

void*
mem_move( void* destination, const void* source, size_t byte_count )
{
    ASSERT_M( destination != nullptr, "Destination is nullptr" );
    ASSERT_M( source != nullptr, "Source is nullptr" );
    ASSERT_M( byte_count > 0, "Bad alloc size" );

    return memmove( destination, source, byte_count );
}

int
mem_cmp( const void* ptr1, const void* ptr2, size_t byte_count )
{
    ASSERT( ptr1 != nullptr );
    ASSERT( ptr2 != nullptr );
    ASSERT( byte_count > 0 );

    return memcmp( ptr1, ptr2, byte_count );
}

void*
mem_realloc( void* ptr, size_t byte_count )
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

void
log( int line, const char* file, const char* func, const char* format, ... )
{
    char buffer[ MAX_BUFFER_LEN ];

    snprintf( buffer, MAX_BUFFER_LEN, "%s: %d\n\t [ %s ] ", file, line, func );

#ifdef __WINDOWS__
	OutputDebugStringA(buffer);
	OutputDebugStringA("\n");
#else
    puts( buffer );
#endif
    va_list args;
    va_start( args, format );
    vsnprintf( buffer, MAX_BUFFER_LEN, format, args );
    va_end( args );

#ifdef __WINDOWS__
	OutputDebugStringA(buffer);
	OutputDebugStringA("\n");
#else
	puts(buffer);
	fflush(stdout);
#endif
}

size_t get_total_memory_usage()
{
    return total_memory_usage;
}

ref_count increment_ref(void *ptr)
{
    ref_count* refs = ptr_minus<ref_count>(ptr, sizeof(ref_count));

    ++(*refs);

	return (*refs);
}

ref_count decrement_ref(void *ptr)
{
    ref_count* refs = ptr_minus<ref_count>(ptr, sizeof(ref_count));

    --(*refs);

	return (*refs);
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
    return basic::mem_allocate( n );
}

void
operator delete( void* p )
{
    basic::mem_free( p );
}

void*
operator new[]( std::size_t s )
{
    return basic::mem_allocate( s );
}

void
operator delete[]( void* p )
{
    basic::mem_free( p );
}
