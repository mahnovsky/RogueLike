#pragma once

#include "debug.hpp"

#include <new>
#include <type_traits>
#include <utility>

namespace basic
{
template < bool B >
struct Token
{
    using Value = decltype( B );
};


template < class T, bool U >
struct Selector
{
	
};

template < class T >
struct Selector<T, true>
{
	static void construct(T* data, uint32 count)
	{
		for(int32 i = 0; i < count; ++i)
		{
			data[i] = T();
		}
	}

	static void destruct(T*, uint32)
	{
	}

	static void copy(T* data, T* const src, uint32 count)
	{
		for (uint32 i = 0; i < count; ++i)
		{
			data[i] = src[i];
		}
	}
};

template < class T >
struct Selector<T*, true>
{
	static void construct(T** data, uint32 count)
	{
		for (uint32 i = 0; i < count; ++i)
		{
			data[i] = nullptr;
		}
	}

	static void destruct(T** , uint32 )
	{
	}

	static void copy(T** data, T** const src, uint32 count)
	{
		for (uint32 i = 0; i < count; ++i)
		{
			data[i] = src[i];
		}
	}
};

template < class T >
struct Selector<T, false>
{
	template <class ... Args>
	static void construct(T* data, uint32 count, Args ... args)
	{
		for (uint32 i = 0; i < count; ++i)
		{
			new (data + i) T( args ...);
		}
	}

	static void destruct(T* data, uint32 count)
	{
		for (uint32 i = 0; i < count; ++i)
		{
			(data + i)->~T();
		}
	}

	static void copy(T* data, T* const src, uint32 count)
	{
		for (uint32 i = 0; i < count; ++i)
		{
			new (static_cast<void*>(data + i)) T(src[i]);
		}
	}
};

struct DefaultAllocator
{
    static uint32
    grow( uint32 capacity )
    {
        return capacity * 2;
    }

    static void*
    alloc_mem( void* ptr, memory_size bytes )
    {
        return mem_realloc( ptr, bytes );
    }

    static void
    free_mem( void* ptr )
    {
        mem_free( ptr );
    }
};

template < class T, class Allocator = DefaultAllocator >
class Vector
{
public:
    using Item = T;
    static constexpr const uint32 MAX_LEN = 0xffffffff;
    static constexpr const uint32 DEFAULT_CAPACITY = 4;

    class Iterator
    {
    public:
        Iterator( Vector< T >& t, uint32 index )
            : m_vector( t )
            , m_index( index )
        {
        }

        T& operator*( )
        {
            return m_vector[ m_index ];
        }

        T* operator->( )
        {
            return &m_vector[ m_index ];
        }

        Iterator& operator++( )
        {
            ++m_index;
            return *this;
        }

        bool
        operator==( const Iterator& other ) const
        {
            return m_index == other.m_index;
        }

        bool
        operator!=( const Iterator& other ) const
        {
            return m_index != other.m_index;
        }

    private:
        Vector< T >& m_vector;
        uint32 m_index;
    };

    class ConstIterator
    {
    public:
        ConstIterator( const Vector< T >& t, uint32 index )
            : m_vector( t )
            , m_index( index )
        {
        }

        const T& operator*( ) const
        {
            return m_vector[ m_index ];
        }

        const T* operator->( ) const
        {
            return &m_vector[ m_index ];
        }

        ConstIterator& operator++( )
        {
            ++m_index;
            return *this;
        }

        bool
        operator==( const ConstIterator& other ) const
        {
            return m_index == other.m_index;
        }

        bool
        operator!=( const ConstIterator& other ) const
        {
            return m_index != other.m_index;
        }

    private:
        const Vector< T >& m_vector;
        uint32 m_index;
    };

	Vector() = default;

    Vector( const Vector< T >& other )
        : m_data( nullptr )
        , m_size( other.m_size )
        , m_capacity( other.m_capacity )
    {
        if ( realloc( ) && m_size > 0 )
        {
			Selector<T, std::is_pod<T>::value>::copy(m_data, other.m_data, m_size);
        }
    }

    Vector( Vector< T >&& other ) noexcept 
		: m_data( other.m_data )
		, m_size( other.m_size )
        , m_capacity( other.m_capacity )
    {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    ~Vector( )
    {
        force_clear( );
    }

    Iterator
    begin( )
    {
        return Iterator( *this, 0 );
    }

    Iterator
    end( )
    {
        return Iterator( *this, get_size( ) );
    }

    ConstIterator
    begin( ) const
    {
        return ConstIterator( *this, 0 );
    }

    ConstIterator
    end( ) const
    {
        return ConstIterator( *this, get_size( ) );
    }

    auto&
    operator=( const Vector< T >& v )
    {
        if ( v.m_size == 0 )
        {
            return *this;
        }

        m_size = v.m_size;
        m_capacity = v.m_capacity;

        if ( realloc( ) && m_size > 0 )
        {
			Selector<T, std::is_pod<T>::value>::copy(m_data, v.m_data, m_size);
        }

        return *this;
    }

    auto&
    operator=( Vector< T >&& v ) noexcept
    {
        force_clear( );

        m_size = v.m_size;
        m_capacity = v.m_capacity;
        m_data = v.m_data;

        v.m_data = nullptr;
        v.force_clear( );

        return *this;
    }

    const T&
    front( ) const
    {
        ASSERT( m_size > 0 );

        return *m_data;
    }

    const T&
    back( ) const
    {
        ASSERT( m_size > 0 );

        return *( m_data + m_size - 1 );
    }

    T&
    front( )
    {
        ASSERT( m_size > 0 );

        return *m_data;
    }

    T&
    back( )
    {
        ASSERT( m_size > 0 );

        return *( m_data + m_size - 1 );
    }

    T*
    get_raw( )
    {
        ASSERT( m_data != nullptr );

        return m_data;
    }

    const T*
    get_raw( ) const
    {
        return m_data;
    }

    uint32
    get_size( ) const
    {
        return m_size;
    }

    uint32
    get_capacity( ) const
    {
        return m_capacity;
    }

    bool
    is_contains( T item )
    {
        uint32 index;

        return find_first( index, item );
    }

    void
    reserve( uint32 count )
    {
        update_capacity( count );

        m_size = ( m_size > count ) ? count : m_size;

        realloc( );
    }

    void
    resize( uint32 count )
    {
		uint32 prev_size = m_size;
        reserve( count );
        m_size = count;
		if (m_size > prev_size)
		{
			Selector<T, std::is_pod_v<T>>::construct(m_data + prev_size, m_size - prev_size);
		}
    }

    void
    fill( T val )
    {
        for ( uint32 index = 0; index < m_size; ++index )
        {
            m_data[ index ] = val;
        }
    }

    bool
    init( const T* ptr, uint32 count )
    {
        ASSERT( ptr != nullptr );
        ASSERT( count > 0 );

        m_size = count;
        update_capacity( m_size );

        if ( realloc( ) )
        {
            uint32 mem_size = count * sizeof( T );
            mem_copy( m_data, ptr, mem_size );

            return true;
        }

        return false;
    }

    template < class F >
    void
    sort( const F& f )
    {
        if ( m_size <= 1 )
        {
            return;
        }

        for ( uint32 i = 0; i < m_size; ++i )
        {
            uint32 index = i;
            for ( uint32 j = i + 1; j < m_size; ++j )
            {
                T& first = m_data[ index ];
                T& second = m_data[ j ];
                if ( f( first, second ) )
                {
                    T tmp = second;
                    m_data[ j ] = first;
                    m_data[ index ] = tmp;
                    index = j;
                }
            }
        }
    }

    void
    append( const T* ptr, uint32 count )
    {
        ASSERT( ptr != nullptr );
        ASSERT( count > 0 );

        uint32 mem_size = sizeof( T ) * count;
        uint32 need_mem_size = sizeof( T ) * m_size + mem_size;

        if ( need_mem_size > ( sizeof( T ) * m_capacity ) )
        {
            reserve( m_size + count );
        }

        T* last = m_data + m_size;
        m_size += count;
        mem_copy( last, ptr, mem_size );
    }

    void
    push( T item )
    {
        if ( !m_data )
        {
            init( );
        }

        uint32 need_size = m_size + 1;
        if ( need_size > m_capacity && !grow( ) )
        {
            ASSERT_M( 0, "Failed push item, realloc was failed" );
            return;
        }

        T* last = m_data + m_size;
		Selector<T, std::is_pod_v<T>>::construct(last, 1);

        ( *last ) = std::move( item );
        ++m_size;
    }

    void
    insert( uint32 index, T item )
    {
        if ( m_size > index )
        {
            m_data[ index ] = item;
        }
        else
        {
            uint32 size = m_size;
            T* data = m_data;
            m_data = nullptr;
            m_size = 0;
            resize( index + 1 );

            if ( data )
            {
                for ( uint32 i = 0; i < size; ++i )
                {
                    m_data[ i ] = data[ i ];
                }
                Allocator::free_mem( data );
            }
            m_data[ index ] = item;
        }
    }

    template < class... Args >
    void emplace( Args... args ) noexcept
    {
        if ( !m_data )
        {
            init( );
        }

        const uint32 need_size = m_size + 1;
        if ( need_size > m_capacity && !grow( ) )
        {
            ASSERT_M( 0, "Failed push item, realloc was failed" );
            return;
        }

        T* last = m_data + m_size;

		Selector<T, std::is_pod_v<T>>::construct(last, 1, args ...);

        ++m_size;
    }

    void
    pop( )
    {
        if ( m_size > 0 )
        {
            --m_size;
        }
    }

    bool
    find_first( uint32& out_index, T value, uint32 pos = 0 ) const
    {
        if ( pos >= m_size )
        {
            return false;
        }

        for ( T* item = m_data + pos; item != ( m_data + m_size ); ++item )
        {
            if ( *item == value )
            {
                out_index = item - m_data;
                return true;
            }
        }

        return false;
    }

    bool
    find_last( uint32& out_index, T value, uint32 pos = MAX_LEN ) const
    {
		if (m_size == 0)
			return false;

        if ( pos > m_size )
        {
            pos = m_size - 1;
        }

        for ( T* item = m_data + pos; item >= m_data; --item )
        {
            if ( *item == value )
            {
                out_index = item - m_data;
                return true;
            }
        }

        return false;
    }

    template < class F >
    bool
    find_if( uint32& out_index, const F f, uint32 pos = 0 ) const
    {
        if ( pos >= m_size )
        {
            return false;
        }

        for ( T* item = m_data + pos; item != ( m_data + m_size ); ++item )
        {
            if ( f( *item ) )
            {
                out_index = item - m_data;
                return true;
            }
        }

        return false;
    }

    bool
    swap_remove( uint32 index )
    {
        ASSERT( index < m_size );
        if ( index == m_size - 1 )
        {
            pop( );
            return false;
        }
        m_data[ index ] = back( );
        pop( );

        return true;
    }

    void
    remove_by_index( uint32 index )
    {
        if ( m_size > index )
        {
            T* pos = m_data + index;
            uint32 move_count = m_size - ( index + 1 );
            Selector< T, std::is_pod<T>::value >::destruct( pos, 1 );
            ASSERT( m_size > 0 );
            if ( m_size > 1 && index < ( m_size - 1 ) )
            {
                mem_move( pos, pos + 1, sizeof( T ) * move_count );
            }
            --m_size;
        }
    }

    void
    remove_by_value( T value, bool all = false )
    {
        if ( m_size < 0 )
        {
            return;
        }

        T* pos = m_data;
        do
        {
            T* end = m_data + m_size;

            if ( *pos == value )
            {
                uint32 index = pos - m_data;

                remove_by_index( index );
            }

            ++pos;
            if ( end == pos || m_size == 0 )
            {
                break;
            }
        } while ( all );
    }

    const T&
    get( uint32 index ) const
    {
        ASSERT( index < m_size );

        return m_data[ index ];
    }

    const T& operator[]( uint32 index ) const
    {
        return get( index );
    }

    T&
    get( uint32 index )
    {
        ASSERT( index < m_size );

        return m_data[ index ];
    }

    T& operator[]( uint32 index )
    {
        return get( index );
    }

    void
    clear( )
    {
		Selector< T, std::is_pod<T>::value >::destruct(m_data, m_size);
        m_size = 0;
    }

    void
    force_clear( )
    {
        if ( m_data )
        {
            clear( );
            m_capacity = 0;
            Allocator::free_mem( m_data );

            m_data = nullptr;
        }
        else
        {
            m_size = 0;
            m_capacity = 0;
        }
    }

    bool
    contains( T value ) const
    {
        basic::uint32 index;
        return find_first( index, value ) > 0;
    }

    bool
    is_empty( ) const
    {
        return m_size == 0;
    }

private:
    void
    update_capacity( uint32 new_size )
    {
        m_capacity = uint32( ( new_size + 1 ) / 2 ) * 2;

        ASSERT( m_capacity >= new_size );
    }

    bool
    realloc( )
    {
        uint32 max_mem_size = m_capacity * sizeof( T );
        T* new_data = static_cast< T* >( Allocator::alloc_mem( m_data, max_mem_size ) );
        if ( !new_data )
        {
            return false;
        }

        m_data = new_data;

        return true;
    }

    void
    init( )
    {
        m_size = 0;
        m_capacity = DEFAULT_CAPACITY;

        realloc( );
    }

    bool
    grow( )
    {
        m_capacity = Allocator::grow( m_capacity );

        return realloc( );
    }

private:
    T* m_data = nullptr;
    uint32 m_size = 0;
    uint32 m_capacity = DEFAULT_CAPACITY;
};

}  // namespace basic
