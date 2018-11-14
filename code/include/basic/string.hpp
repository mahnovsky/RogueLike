#pragma once

#include "util_functions.hpp"
#include "vector.hpp"

namespace basic
{
template < class T >
class BaseString
{
public:
    static const size_t max_len = 0xffffffff;
    static const T cstr_end = 0;

    BaseString( )
        : m_buffer( )
    {
    }

    BaseString( const T* cstr )
        : m_buffer( )
    {
        init( cstr );
    }

    BaseString( const T* cstr, size_t count )
        : m_buffer( )
    {
        ASSERT( str_length( cstr, max_len ) > count );
        m_buffer.init( cstr, count );
        if ( m_buffer.back( ) != cstr_end )
        {
            m_buffer.push( cstr_end );
        }
    }

    BaseString( const BaseString< T >& other )
        : m_buffer( other.m_buffer )
    {
    }

    BaseString( BaseString< T >&& other )
        : m_buffer( )
    {
        m_buffer = std::move( Vector< T >( other.m_buffer ) );
    }

    void
    init( const T* cstr )
    {
        m_buffer.init( cstr, str_length( cstr, max_len ) + 1 );
    }

    void
    append( const T* cstr )
    {
        ASSERT( m_buffer.back( ) == cstr_end );

        m_buffer.pop( );
        m_buffer.append( cstr, str_length( cstr, max_len ) + 1 );
    }

    bool
    is_empty( ) const
    {
        return m_buffer.is_empty( );
    }

    const T*
    get_cstr( ) const
    {
        return m_buffer.get_raw( );
    }

    BaseString< T >&
    operator=( const BaseString< T >& str )
    {
        init( str.get_cstr( ) );

        return *this;
    }

    BaseString< T >&
    operator=( const T* cstr )
    {
        init( cstr );

        return *this;
    }

    BaseString< T >
    operator+( const T* cstr ) const
    {
        BaseString< T > res( get_cstr( ) );

        res.append( cstr );

        return std::move( res );
    }

    void
    operator+=( const T* cstr )
    {
        append( cstr );
    }

    T operator[]( size_t index ) const
    {
        ASSERT( index < m_buffer.get_size( ) );

        return m_buffer[ index ];
    }

    bool
    find_first( size_t& out_index, T value, size_t pos = 0 ) const
    {
        return m_buffer.find_first( out_index, value, pos );
    }

    bool
    find_last( size_t& out_index, T value, size_t pos = Vector< T >::MAX_LEN ) const
    {
        return m_buffer.find_last( out_index, value, pos );
    }

    BaseString< T >
    get_substr( size_t pos, size_t count )
    {
        ASSERT( ( pos + count ) < m_buffer.get_size( ) );

        BaseString< T > res( get_cstr( ) + pos, count );

        return std::move( res );
    }

    size_t
    get_size( ) const
    {
        return m_buffer.get_size( );
    }

    void
    split( basic::Vector< BaseString< T > >& out, T item )
    {
        size_t pos = 0;
        size_t next_index = max_len;

        while ( pos < get_size( ) )
        {
            if ( !find_first( next_index, item, pos ) )
            {
                out.push( std::move( get_substr( pos, get_size( ) - pos ) ) );
                break;
            }

            out.push( std::move( get_substr( pos, next_index - pos ) ) );

            pos = next_index + 1;
        }
    }

    template < class Type, class Convert >
    void
    split_to( basic::Vector< Type >& out, T item, Convert convert_func )
    {
        size_t pos = 0;
        size_t next_index = max_len;

        while ( pos < get_size( ) )
        {
            if ( !find_first( next_index, item, pos ) )
            {
                Type t = convert_func( std::move( get_substr( pos, get_size( ) - pos ) ) );
                out.push( std::move( t ) );
                break;
            }

            Type t = convert_func( std::move( get_substr( pos, next_index - pos ) ) );

            out.push( std::move( t ) );

            pos = next_index + 1;
        }
    }

    T
    front( ) const
    {
        return m_buffer.front( );
    }

    T
    back( ) const
    {
        return m_buffer.back( );
    }

    static BaseString< T >
    read_line( T* cstr, size_t max_size )
    {
        BaseString< T > result;

        for ( size_t i = 0; i < max_size; ++i )
        {
            T item = *( cstr + i );
            const T next_line = static_cast< T >( '\n' );

            if ( item == next_line )
            {
                break;
            }

            result.m_buffer.push( item );
        }

        return std::move( result );
    }

private:
    Vector< T > m_buffer;
};

template < class T >
bool
operator==( const BaseString< T >& s1, const BaseString< T >& s2 )
{
    if ( s1.get_size( ) != s2.get_size( ) )
    {
        return false;
    }

    return mem_cmp( s1.get_cstr( ), s2.get_cstr( ), s2.get_size( ) ) == 0;
}

template < class T >
bool
operator==( const BaseString< T >& s1, const T* s2 )
{
    ASSERT( s2 != nullptr );

    size_t size = str_length< T >( s2, basic::BaseString< T >::max_len ) + 1;

    if ( s1.get_size( ) != size )
    {
        return false;
    }

    return mem_cmp( s1.get_cstr( ), s2, size ) == 0;
}

using String = BaseString< char >;
}
