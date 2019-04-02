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

template < class T >
struct Initializer
{
    template < class... Args >
    static void
    construct( T*, Token< true >, Args...  )
    {
    }

    static void
    construct( T*, Token< true > )
    {
    }

    static void
    destruct( T*, Token< true > )
    {
    }

    static void
    copy( T* , const T* , Token< true > )
    {
    }

    static void
    construct( T* ptr, Token< false > )
    {
        new ( static_cast< void* >( ptr ) ) T( );
    }

    static void
    destruct( T* ptr, Token< false > )
    {
        ptr->~T( );
    }

    static void
    copy( T* ptr, const T* src, Token< false > )
    {
        const T& ref = *src;

        new ( static_cast< void* >( ptr ) ) T( ref );
    }

    template < class... Args >
    static void
    construct( T* ptr, Token< false >, Args... args )
    {
        new ( static_cast< void* >( ptr ) ) T( args... );
    }
};

struct DefaultAllocator
{
    static uint32 grow( uint32 capacity ) { return capacity * 2; }

    static void* alloc_mem( void* ptr, memory_size bytes ) { return mem_realloc( ptr, bytes ); }

    static void free_mem( void* ptr ) { mem_free( ptr ); }
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
        Iterator(Vector<T>& t, uint32 index)
            :m_vector(t)
            ,m_index(index)
        {}

        T& operator *()
        {
            return m_vector[m_index];
        }

        T* operator ->()
        {
            return &m_vector[m_index];
        }

        Iterator& operator ++ ()
        {
            ++m_index;
            return *this;
        }

        bool operator == (const Iterator& other) const
        {
            return m_index == other.m_index;
        }

        bool operator != (const Iterator& other) const
        {
            return m_index != other.m_index;
        }

    private:
        Vector<T>& m_vector;
        uint32 m_index;
    };

    class ConstIterator
    {
    public:
        ConstIterator(const Vector<T>& t, uint32 index)
            :m_vector(t)
            ,m_index(index)
        {}

        const T& operator *() const
        {
            return m_vector[m_index];
        }

        const T* operator ->() const
        {
            return &m_vector[m_index];
        }

        ConstIterator& operator ++ ()
        {
            ++m_index;
            return *this;
        }

        bool operator == (const ConstIterator& other) const
        {
            return m_index == other.m_index;
        }

        bool operator != (const ConstIterator& other) const
        {
            return m_index != other.m_index;
        }

    private:
        const Vector<T>& m_vector;
        uint32 m_index;
    };

    Vector( )
        : m_data( nullptr )
        , m_size( 0 )
        , m_capacity( DEFAULT_CAPACITY )
    {
    }

    Vector( const Vector< T >& other )
        : m_data( nullptr )
        , m_size( other.m_size )
        , m_capacity( other.m_capacity )
    {
        if ( realloc( ) && m_size > 0 )
        {
            mem_copy( m_data, other.m_data, sizeof( T ) * other.m_size );
            copy_elements( other.m_data, 0 );
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

    Iterator begin()
    {
        return Iterator(*this, 0);
    }

    Iterator end()
    {
        return Iterator(*this, get_size());
    }

    ConstIterator begin() const
    {
        return ConstIterator(*this, 0);
    }

    ConstIterator end() const
    {
        return ConstIterator(*this, get_size());
    }

    Vector< T >& operator=( const Vector< T >& v )
    {
        if(v.m_size == 0)
        {
            return *this;
        }

        m_size = v.m_size;
        m_capacity = v.m_capacity;

        if ( realloc( ) )
        {
            mem_copy( m_data, v.m_data, sizeof( T ) * m_size );
            copy_elements( v.m_data, 0 );
        }

        return *this;
    }

    Vector< T >& operator=( Vector< T >&& v ) noexcept
    {
        force_clear();

        m_size = v.m_size;
        m_capacity = v.m_capacity;
        m_data = v.m_data;

        v.m_data = nullptr;
        v.force_clear( );

        return *this;
    }

    const T& front( ) const
    {
        ASSERT( m_size > 0 );

        return *m_data;
    }

    const T& back( ) const
    {
        ASSERT( m_size > 0 );

        return *( m_data + m_size - 1 );
    }

    T& front( )
    {
        ASSERT( m_size > 0 );

        return *m_data;
    }

    T& back( )
    {
        ASSERT( m_size > 0 );

        return *( m_data + m_size - 1 );
    }

    T* get_raw( )
    {
        ASSERT( m_data != nullptr );

        return m_data;
    }

    const T* get_raw( ) const
    {
        return m_data;
    }

    uint32 get_size( ) const
    {
        return m_size;
    }

    uint32 get_capacity() const
    {
        return  m_capacity;
    }

    bool is_contains(T item)
    {
        uint32 index;

        return find_first( index, item );
    }

    void reserve( uint32 count )
    {
        update_capacity( count );

        m_size = (m_size > count) ? count : m_size;

        realloc( );
    }

    void resize( uint32 count )
    {
        reserve( count );
        m_size = count;

        construct_elements( 0 );
    }

    bool init( const T* ptr, uint32 count )
    {
        ASSERT( ptr != nullptr );
        ASSERT( count > 0 );

        m_size = count;
        update_capacity( m_size );

        if ( realloc( ) )
        {
            uint32 mem_size = count * sizeof( T );
            mem_copy( m_data, ptr, mem_size );

            construct_elements( 0 );

            return true;
        }

        return false;
    }

    void append( const T* ptr, uint32 count )
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

    void push( T item )
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
        Initializer< T >::construct( last, Token< std::is_pod< T >::value >( ) );
        ( *last ) = std::move( item );
        ++m_size;
    }

    template < class... Args >
    void emplace( Args... args )
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
        Initializer< T >::construct( last, Token< std::is_pod< T >::value >( ), args... );
        ++m_size;
    }

    void pop( )
    {
        if ( m_size > 0 )
        {
            --m_size;
        }
    }

    bool find_first( uint32& out_index, T value, uint32 pos = 0 ) const
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

    bool find_last( uint32& out_index, T value, uint32 pos = MAX_LEN ) const
    {
        if ( pos > m_size )
        {
            pos = m_size - 1;
        }

        for ( T* item = m_data + pos; item != m_data; --item )
        {
            if ( *item == value )
            {
                out_index = item - m_data;
                return true;
            }
        }

        return false;
    }

    bool swap_remove( uint32 index )
    {
		ASSERT(index < m_size);
		if (index == m_size - 1)
		{
			pop();
			return false;
		}
        m_data[index] = back();
        pop();

		return true;
    }

    void remove_by_index( uint32 index )
    {
        if ( m_size > index )
        {
            T* pos = m_data + index;
            uint32 move_count = m_size - ( index + 1 );
            Initializer< T >::destruct( pos, Token< std::is_pod< T >::value >( ) );

            if ( m_size > 1 )
            {
                mem_move( pos, pos + 1, sizeof( T ) * move_count );
            }
            --m_size;
        }
    }

    void remove_by_value( T value, bool all = false )
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
        }
        while ( all );
    }

    const T& get( uint32 index ) const
    {
        ASSERT( index < m_size );

        return m_data[ index ];
    }

    const T& operator[]( uint32 index ) const
    {
        return get( index );
    }

    T& get( uint32 index )
    {
        ASSERT( index < m_size );

        return m_data[ index ];
    }

    T& operator[]( uint32 index )
    {
        return get( index );
    }

    void clear( )
    {
        destruct_elements( 0 );
        m_size = 0;
    }

    void force_clear( )
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
    void update_capacity( uint32 new_size )
    {
        m_capacity = uint32((new_size + 1) / 2) * 2;

        ASSERT( m_capacity >= new_size );
    }

    bool realloc( )
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

    void init( )
    {
        m_size = 0;
        m_capacity = DEFAULT_CAPACITY;

        realloc( );
    }

    bool grow( )
    {
        m_capacity = Allocator::grow( m_capacity );

        return realloc( );
    }

    void construct_elements( uint32 pos )
    {
        for ( uint32 i = pos; i < m_size; ++i )
        {
            Initializer< T >::construct( m_data + i, Token< std::is_pod< T >::value >( ) );
        }
    }

    void destruct_elements( uint32 pos )
    {
        for ( uint32 i = pos; i < m_size; ++i )
        {
            Initializer< T >::destruct( m_data + i, Token< std::is_pod< T >::value >( ) );
        }
    }

    void copy_elements( const T* src, uint32 pos )
    {
        for ( uint32 i = pos; i < m_size; ++i )
        {
            Initializer< T >::copy( m_data + i, src + i, Token< std::is_pod< T >::value >( ) );
        }
    }

private:
    T* m_data;
    uint32 m_size;
    uint32 m_capacity;
};

}  // namespace basic
