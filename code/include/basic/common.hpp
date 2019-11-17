#pragma once

#include <memory>
#include "debug.hpp"
#include "file.hpp"
#include "hash_map.hpp"
#include "image.hpp"
#include "json_document.hpp"
#include "memory.hpp"
#include "pool.hpp"
#include "string.hpp"
#include "time.hpp"
#include "vector.hpp"

template < class T, class U >
T
size_as( const basic::Vector< U >& v )
{
    return static_cast< T >( v.get_size( ) );
}

template < class U >
basic::uint32
size( const basic::Vector< U >& v )
{
    return v.get_size( );
}

template <class T>
basic::uint32 constexpr enum2num(T e)
{
	return static_cast<basic::uint32>(e);
}

#define MAX_COUNTER_VALUE 32

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-w"

namespace basic
{
template < int N >
struct flag
{
    friend constexpr int adl_flag( flag< N > );
};

template < int N >
struct writer
{
    friend constexpr int adl_flag( flag< N > )
    {
        static_assert( N < MAX_COUNTER_VALUE, "Error max value" );
        return N;
    }

    static constexpr int value = N;
};
template < int N, int = adl_flag( flag< N >{} ) >
int constexpr reader( int, flag< N > )
{
    return N;
}

template < int N >
int constexpr reader( float, flag< N >, int R = reader( 0, flag< N - 1 >{} ) )
{
    return R;
}

int constexpr reader( float, flag< 0 > )
{
    return 0;
}

template < int N = 1 >
int constexpr next( int R = writer< reader( 0, flag< MAX_COUNTER_VALUE >{} ) + N >::value )
{
    return R;
}
}

#pragma GCC diagnostic pop

//#define DECLARE_TYPE_UID static constexpr basic::int32 TYPE_UID = basic::next( )
#define DECLARE_TYPE_UID static constexpr basic::int32 TYPE_UID = __COUNTER__

#else

#define DECLARE_TYPE_UID static constexpr basic::int32 TYPE_UID = __COUNTER__

#endif

#define GENERATE_COMP( name, type, field_type, field ) \
    struct name                                        \
    {                                                  \
        const field_type& field;                       \
        name( const field_type& ft )                   \
            : field( ft )                              \
        {                                              \
        }                                              \
        bool                                           \
        operator( )( const type& t ) const             \
        {                                              \
            return t.field == field;                   \
        }                                              \
    };

#define GENERATE_GREATER_COMP_PTR( name, type, field )      \
    struct name                                             \
    {                                                       \
        bool                                                \
        operator( )( const type* t1, const type* t2 ) const \
        {                                                   \
            return t1->field > t2->field;                   \
        }                                                   \
    };
