#pragma once

#include "basic/string.hpp"

template < class T >
struct DefDeleter
{
    void
    operator( )( T* obj )
    {
        delete obj;
    }
};

template < class T, class Deleter = DefDeleter< T > >
class Cache
{
public:
    using Handle = size_t;
    static const Handle INVALID_HANDLE = 0xFFFFFFFF;

    struct ObjectInfo
    {
        basic::String name;
        Handle handle;
        T* object;

        ObjectInfo( const char* n, Handle h, T* o )
            : name( n )
            , handle( h )
            , object( o )
        {
        }
    };

    Cache( Deleter d = DefDeleter< T >( ) )
        : m_objects( )
        , m_handle_counter( 0 )
        , m_deleter( d )
    {
    }

    ~Cache( )
    {
        clean( );
    }

    void
    clean( )
    {
        for ( size_t i = 0; i < m_objects.get_size( ); ++i )
        {
            m_deleter( m_objects[ i ].object );
        }
    }

    bool
    add( T* object, const char* name, Handle* out_handle = nullptr )
    {
        Handle handle = get_handle( name );

        if ( handle == INVALID_HANDLE )
        {
            Handle handle = m_handle_counter++;
            if ( out_handle )
            {
                ( *out_handle ) = handle;
            }

            m_objects.emplace( name, handle, object );

            return true;
        }

        return false;
    }

    void
    remove( Handle handle )
    {
        size_t index = 0;
        if ( find( handle, index ) )
        {
            m_objects.remove_by_index( index );
        }
    }

    const T*
    get( Handle handle ) const
    {
        return get( handle );
    }

    T*
    get( Handle handle )
    {
        size_t index = 0;
        if ( find( handle, index ) )
        {
            return m_objects[ index ].object;
        }

        return nullptr;
    }

    Handle
    get_handle( const char* name )
    {
        basic::String sname = name;

        for ( size_t i = 0; i < m_objects.get_size( ); ++i )
        {
            if ( m_objects[ i ].name == sname )
            {
                return m_objects[ i ].handle;
            }
        }

        return INVALID_HANDLE;
    }

private:
    bool
    find( Handle handle, size_t& out_index ) const
    {
        for ( size_t i = 0; i < m_objects.get_size( ); ++i )
        {
            if ( m_objects[ i ].handle == handle )
            {
                return true;
            }
        }

        return false;
    }

private:
    basic::Vector< ObjectInfo > m_objects;
    size_t m_handle_counter;
    Deleter m_deleter;
};
