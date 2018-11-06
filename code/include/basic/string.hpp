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

using String = BaseString< char >;
}
