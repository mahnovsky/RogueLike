#include "debug.hpp"

#include "stb/stb_sprintf.h"

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
void
assert_func( int line, const char* file, const char* function, const char* message )
{
    LOG( "Assertion failed!" );
	LOG( "\t%s : %d\n", file, line );
	LOG( "\tfunction: %s\n", function );
	LOG( "\tmessage: %s\n", message );
    abort( );
}

uint32 str_length(const char* cstring, memory_size max_len )
{
    ASSERT_M( cstring != nullptr, "cstring is nullptr" );
    ASSERT_M( max_len > 0, "max_len must be > 0" );

    return static_cast<uint32>( strnlen( cstring, max_len ) );
}

char* str_copy( const char* cstr, memory_size max_len )
{
    size_t size = str_length( cstr, max_len );

    char* res = static_cast< char* >( mem_alloc( size ) );
    mem_copy( res, cstr, size );

    if ( size == max_len )
    {
        res[ size ] = 0;
    }

    return res;
}

void log( int line, const char* file, const char* const func, const char* format, ... )
{
    char buffer[ MAX_BUFFER_LEN ];

    stbsp_snprintf( buffer, MAX_BUFFER_LEN, "[%s: %d] %s: ", file, line, func );

#ifdef __WINDOWS__
	OutputDebugStringA(buffer);
	OutputDebugStringA("\n\t");
#else
    puts( buffer );
#endif
    va_list args;
    va_start( args, format );
    stbsp_vsnprintf( buffer, MAX_BUFFER_LEN, format, args );
    va_end( args );

#ifdef __WINDOWS__
	OutputDebugStringA(buffer);
	OutputDebugStringA("\n\n");
#else
	puts(buffer);
	fflush(stdout);
#endif
}

}
