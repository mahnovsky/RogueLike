#include "basic/util_functions.hpp"

#include <cstdlib>
#include <cstdio>
#include <cstring>

#define MAX_BUFFER_LEN 1024

namespace basic
{

void assert_func( int line, const char* file, const char* function, const char* message )
{
    puts("Assertion failed!");
    printf( "\t%s : %d\n", file, line );
    printf( "\tfunction: %s\n", function );
    printf( "\tmessage: %s\n", message );
    abort();
}

size_t str_length( const char* cstring, size_t max_len )
{
    ASSERT_M( cstring != nullptr, "cstring is nullptr" );
    ASSERT_M( max_len > 0, "max_len must be > 0" );

    return strnlen( cstring, max_len );
}

void* mem_copy( void* destination, const void* source, size_t byte_count )
{
    ASSERT_M( destination != nullptr, "Destination is nullptr" );
    ASSERT_M( source != nullptr, "Source is nullptr" );
    ASSERT_M( byte_count > 0, "Bad alloc size" );

    return memcpy( destination, source, byte_count );
}

void* mem_allocate( size_t byte_count )
{
    ASSERT_M( byte_count > 0, "Bad alloc size" );

    return malloc( byte_count );
}

void mem_free( void* mem )
{
    free( mem );
}

void* mem_move( void* destination, const void* source, size_t byte_count )
{
    ASSERT_M( destination != nullptr, "Destination is nullptr" );
    ASSERT_M( source != nullptr, "Source is nullptr" );
    ASSERT_M( byte_count > 0, "Bad alloc size" );

    return memmove( destination, source, byte_count );
}

int mem_cmp( const void* ptr1, const void* ptr2, size_t byte_count )
{
    ASSERT( ptr1 != nullptr );
    ASSERT( ptr2 != nullptr );
    ASSERT( byte_count > 0 );

    return memcmp( ptr1, ptr2, byte_count );
}

void* mem_realloc( void* ptr, size_t byte_count )
{
    ASSERT( byte_count > 0 );

    return realloc( ptr, byte_count );
}

void
log( int line,
     const char* file,
     const char* func,
     const char* format,
     ... )
{
    char buffer[ MAX_BUFFER_LEN ];

    snprintf( buffer, MAX_BUFFER_LEN, "%s: %d\n\t [ %s ] ", file , line, func );

    puts( buffer );

    va_list args;
    va_start( args, format );
    vsnprintf( buffer, MAX_BUFFER_LEN, format, args );
    va_end( args );

    puts( buffer );
    fflush( stdout );
}


}
