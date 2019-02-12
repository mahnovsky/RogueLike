#pragma once

#include "memory.hpp"

namespace basic
{

void assert_func( int line, const char* file, const char* function, const char* message );

memory_size str_length( const char* ptr, memory_size max_len );

char* str_copy(const char* cstr, memory_size max_len );

void log(int line, const char* file, const char * const func, const char* format, ... );

}

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
