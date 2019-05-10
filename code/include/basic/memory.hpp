#pragma once

#include "types.hpp"

#ifdef _DEBUG

#define mem_alloc( bytes ) _checked_mem_alloc( bytes, __FILE__, __LINE__ )
#define mem_realloc( ptr, bytes ) _checked_mem_realloc( ptr, bytes, __FILE__, __LINE__ )
#define mem_free _mem_free

#elif 0
#include <stdio.h>

#define mem_alloc wrapper_malloc
#define mem_realloc wrapper_realloc
#define mem_free wrapper_free

#else

//#define mem_alloc _mem_alloc
//#define mem_realloc _mem_realloc

#define mem_alloc( bytes ) _checked_mem_alloc( bytes, __FILE__, __LINE__ )
#define mem_realloc( ptr, bytes ) _checked_mem_realloc( ptr, bytes, __FILE__, __LINE__ )
#define mem_free _mem_free

#endif

namespace basic
{
using memory_size = uint32;
using ref_count = int32;
using mem_out_callback = void ( * )( );

void* _mem_alloc( memory_size bytes );

void* _mem_realloc( void* ptr, memory_size bytes );

void _mem_free( void* mem );

void* _checked_mem_alloc( memory_size bytes, const char* file, int line );

void* _checked_mem_realloc( void* ptr, memory_size bytes, const char* file, int line );

void* mem_move( void* destination, const void* source, memory_size bytes );

void* mem_copy( void* destination, const void* source, memory_size bytes );

int mem_cmp( const void* ptr1, const void* ptr2, memory_size bytes );

void mem_set_out_of_memory( mem_out_callback callback );

size_t get_memory_usage( );

void* wrapper_malloc( size_t size );

void* wrapper_realloc( void* ptr, size_t size );

void wrapper_free( void* ptr );

template < class T >
uint32
size_of( )
{
    return sizeof( T );
}

template < class T, class T1, class... Args >
uint32
size_of( )
{
    return sizeof( T ) + size_of< T1, Args... >( );
}

template < typename... Args >
void*
alloc_objects( const char* file, int line )
{
    uint32 size = size_of< Args... >( );  //::size();

    return _checked_mem_alloc( size, file, line );
}

template < typename T, typename... Args >
T*
init_object( void* ptr, uint32& offset, Args... args )
{
    uint32 off = offset;
    offset += sizeof( T );
    char* cptr = static_cast< char* >( ptr );
    return new ( cptr + off ) T( args... );
}

template < typename T >
void
delete_obj( T* ptr )
{
    if ( ptr )
    {
        ptr->~T( );
        mem_free( ptr );
    }
}
}

void* operator new( std::size_t n );
void operator delete( void* p ) noexcept;

void* operator new[]( std::size_t s );
void operator delete[]( void* p ) noexcept;

#define ALLOC_OBJECTS( ... ) basic::alloc_objects< __VA_ARGS__ >( __FILE__, __LINE__ )
#define NEW_OBJ( TYPE, ... ) new ( basic::mem_alloc( sizeof( TYPE ) ) ) TYPE( __VA_ARGS__ )
#define DELETE_OBJ( ptr ) basic::delete_obj( ptr )
