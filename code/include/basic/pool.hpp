#pragma once

#include "vector.hpp"

namespace basic
{
template < class T >
class Pool
{
public:
    struct Buffer
    {
        basic::uint32 offset;
        void* ptr;
        Buffer( basic::uint32 o, void* p )
            : offset( o )
            , ptr( p )
        {
        }
    };

    static const basic::uint32 MIN_CHUNK_SIZE = 10;

    Pool( )
        : Pool( sizeof( T ) * 100 )
    {
    }

    Pool( basic::uint32 size )
        : m_buffer_size( sizeof( T ) * size )
        , m_buffers( )
        , m_free_objects( )
    {
        add_buffer( );
    }

    ~Pool( )
    {
        for ( auto& b : m_buffers )
        {
            mem_free( b.ptr );
        }
    }

    T*
    make( )
    {
        T* obj = nullptr;
        if ( !m_free_objects.is_empty( ) )
        {
            T* obj = m_free_objects.back( );
            m_free_objects.pop( );
            return obj;
        }

        Buffer& b = m_buffers.back( );
        if ( b.offset >= m_buffer_size )
        {
            b = add_buffer( );
        }

        obj = static_cast< T* >( b.ptr + b.offset );
        b.offset += sizeof( T );

        return obj;
    }

    void
    free( T* t )
    {
        m_free_objects.push( t );
    }

    Buffer&
    add_buffer( )
    {
        m_buffers.emplace( 0, mem_alloc( m_buffer_size ) );

        return m_buffers.back( );
    }

private:
    const basic::uint32 m_buffer_size;
    basic::Vector< Buffer > m_buffers;
    basic::Vector< T* > m_free_objects;
};
}
