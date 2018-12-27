#pragma once

#include "types.hpp"

namespace basic
{
void assert_func( int line, const char* file, const char* function, const char* message );

size_t str_length( const char* ptr, size_t max_len );

char* str_copy( const char* cstr, size_t max_len );

void* mem_copy( void* destination, const void* source, size_t byte_count );

void* mem_allocate( size_t byte_count );

void mem_free( void* mem );

void* mem_move( void* destination, const void* source, size_t byte_count );

int mem_cmp( const void* ptr1, const void* ptr2, size_t byte_count );

void* mem_realloc( void* ptr, size_t byte_count );

using mem_out_callback = void ( * )( );

void mem_set_out_of_memory( mem_out_callback callback );

void log( int line, const char* file, const char* func, const char* format, ... );

template < class T >
size_t
str_length( const T* cstring, size_t max_len )
{
    const T* pos = cstring;
    size_t offset = 0;
    while ( *( pos + offset ) != 0 && ( offset < max_len ) )
        ++offset;

    return offset;
}
}

void* operator new( std::size_t n );
void operator delete( void* p );

#define LOG( ... ) basic::log( __LINE__, __FILE__, __PRETTY_FUNCTION__, __VA_ARGS__ )

#ifdef _DEBUG
#define ASSERT( cond )                                                        \
    if ( !( cond ) )                                                          \
    {                                                                         \
        basic::assert_func( __LINE__, __FILE__, __PRETTY_FUNCTION__, #cond ); \
    }
#define ASSERT_M( cond, message )                                               \
    if ( !( cond ) )                                                            \
    {                                                                           \
        basic::assert_func( __LINE__, __FILE__, __PRETTY_FUNCTION__, message ); \
    }
#else
#define ASSERT( cond ) ;
#define ASSERT_M( cond, msg ) ;
#endif
