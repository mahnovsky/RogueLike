#pragma once

#include "util_functions.hpp"

#include <utility>
#include <type_traits>
#include <new>

namespace basic
{

template <bool B>
struct Token
{
    using Value = decltype(B);
};

template <class T>
struct Initializer
{
    template <class ... Args>
    static void construct( T*, Token<true>, Args ... args )
    {}

    static void construct( T* ptr, Token<true> ){} 

    static void destruct( T* ptr, Token<true> ){}

    static void copy( T* ptr, const T* src, Token<true> ){}

    static void construct( T* ptr, Token<false> ) 

    {
        new(static_cast<void*>(ptr)) T();
    }

    static void destruct( T* ptr, Token<false> )
    {
        ptr->~T();
    }
    
    static void copy( T* ptr, const T* src, Token<false> ) 
    {
        ptr->T(*src);
    }

    template <class ... Args>
    static void construct( T* ptr, Token<false>, Args ... args )
    {
        new(static_cast<void*>(ptr)) T( args ... );
    }

};

template <class T, size_t Capacity = 8>
class Vector
{
public:
    Vector()
        :m_data(nullptr)
        ,m_size(0)
        ,m_max_size(Capacity)
    {}

    Vector( const Vector<T>& other )
        :m_data(nullptr)
        ,m_size( other.m_size )
        ,m_max_size( other.m_max_size )
    {
        if( realloc() && m_size > 0 )
        {
            mem_copy( m_data, other.m_data, sizeof(T) * other.m_size );            
            copy_elements( other.m_data, 0 );
        }
    }

    Vector( Vector<T>&& other )
        :m_data( other.m_data )
        ,m_size( other.m_size )
        ,m_max_size( other.m_max_size )
    {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_max_size = 0;
    }

    ~Vector()
    {
        force_clear( );
    }

    Vector<T>& operator = ( const Vector<T>& v )
    {
        m_size = v.m_size;
        m_max_size = v.m_max_size;
        
        if( realloc() )
        {
            mem_copy( m_data, v.m_data, sizeof(T) * m_size );
            copy_elements( v.m_data, 0 );
        }
 
        return *this;
    }

    Vector<T>& operator = ( Vector<T>&& v )
    {
        m_size = v.m_size;
        m_max_size = v.m_max_size;
        m_data = v.m_data;

        v.m_data = nullptr;
        v.force_clear();

        return *this;
    }

    T front() const
    {
        ASSERT( m_size > 0 );

        return *m_data;
    }

    T back( ) const 
    {
        ASSERT( m_size > 0 );

        return *( m_data + m_size - 1 );
    }

    T* get_raw() 
    {
        ASSERT( m_data != nullptr );

        return m_data;
    }

    const T* get_raw() const
    {
        return m_data;
    }

    size_t get_size() const
    {
        return m_size;
    }

    void reserve( size_t count )
    {
        size_t chunks = ( count / Capacity ) + 1;
        m_max_size = chunks * Capacity;
        m_size = m_size > count ? count : m_size;
        
        realloc();
    }

    void resize( size_t count )
    {
        reserve( count );
        m_size = count;

        construct_elements(0);
    }

    
    bool init( const T* ptr, size_t count )
    {
        ASSERT( ptr != nullptr );
        ASSERT( count > 0 );

        m_size = count;
        size_t chuncks = ( m_size / Capacity ) + 1;
        m_max_size = chuncks * Capacity;

        ASSERT( m_max_size > m_size );

        if ( realloc() )
        {
            size_t mem_size = count * sizeof(T);
            mem_copy( m_data, ptr, mem_size );

            construct_elements(0);
            
            return true;
        }

        return false;
    }

    void append( const T* ptr, size_t count )
    {
        ASSERT( ptr != nullptr );
        ASSERT( count > 0 );

        size_t mem_size = sizeof(T) * count;
        size_t need_mem_size = sizeof(T) * m_size + mem_size;

        if( need_mem_size > ( sizeof(T) * m_max_size ) )
        {
            reserve( m_size + count );
        }

        T* last = m_data + m_size;
        m_size += count;
        mem_copy( last, ptr, mem_size );
    }

    void push( T item )
    {
        if( !m_data )
        {
            init();
        }

        size_t need_size = m_size + 1;
        if( need_size > m_max_size && !grow() )
        {
            ASSERT_M( 0, "Failed push item, realloc was failed" );
            return;
        }

        T* last = m_data + m_size;
        Initializer<T>::construct( last, Token<std::is_pod<T>::value>() );
        (*last) = item;
        ++m_size;
    }

    template < class ... Args >
    void emplace( Args ... args )
    {
        if( !m_data )
        {
            init();
        }

        size_t need_size = m_size + 1;
        if( need_size > m_max_size && !grow() )
        {
            ASSERT_M( 0, "Failed push item, realloc was failed" );
            return;
        }

        T* last = m_data + m_size;
        Initializer<T>::construct( last, Token<std::is_pod<T>::value>(), args ... );
        ++m_size;
    }


    void pop()
    {
        if( m_size > 0 )
        {
            --m_size;
        }
    }

    int find_first( T value ) const
    {
        for( T* pos = m_data; pos != (m_data + m_size); ++pos)
        {
            if( *pos == value )
            {
                return pos - m_data;
            }
        }

        return -1;
    }

    int find_last( T value ) const
    {
        for( T* pos = m_data + m_size; pos != m_data; --pos )
        {
            if( *pos == value )
            {
                return pos - m_data;
            }
        }

        return -1;
    }

    void remove_by_index( size_t index )
    {
        if( m_size > index )
        {
            T* pos = m_data + index;
            size_t move_count = m_size - (index + 1);
            Initializer<T>::destruct( pos, Token<std::is_pod<T>::value >() );

            if( m_size > 1 )
            {
                mem_move(pos, pos + 1, sizeof(T) * move_count );
            }
            --m_size;
        }
    }

    void remove_by_value( T value, bool all = false )
    {   
        if( m_size < 0 )
        {
            return;
        }

        T* pos = m_data;     
        do
        {
            T* end = m_data + m_size;

            if( *pos == value )
            {
                size_t index = pos - m_data;

                remove_by_index( index );
            }
            
            ++pos;
            if( end == pos || m_size == 0)
            {
                break;
            }
        }
        while( all );
    }

    const T& get( size_t index ) const
    {
        ASSERT( index < m_size ); 

        return m_data[ index ];
    }

    const T operator []( size_t index ) const
    {
        return get( index ); 
    }

    T& get( size_t index )
    {
        ASSERT( index < m_size );

        return m_data[ index ];
    }

    T& operator [] ( size_t index )
    {
        return get( index );
    }

    void clear()
    {
        destruct_elements( 0 );
        m_size = 0;        
    }

    void force_clear( )
    {
        clear();
        m_max_size = 0;
        if( m_data )
        {
            mem_free( m_data );
            m_data = nullptr;
        }
    }
    
    void shrink()
    {
        size_t max_count = m_max_size / Capacity;
        size_t curr_count = m_size / Capacity;
        if( max_count > curr_count )
        {
            size_t new_count = max_count - curr_count;
            m_max_size = Capacity * new_count;
            T* new_data = static_cast<T*>( mem_allocate( sizeof(T) * m_max_size ) );
            mem_copy( new_data, m_data, sizeof(T) * m_size );
            mem_free( m_data );
            m_data = new_data;
        }
    }

    bool contains( T value ) const
    {
        return find_first( value ) > 0;
    }

    bool is_empty() const
    {
        return m_size == 0;
    }

private:
    bool realloc()
    {
        size_t max_mem_size = m_max_size * sizeof(T);
        T* new_data = static_cast<T*>( mem_realloc( m_data, max_mem_size ) );  
        if( !new_data )
        {
            return false;
        }

        m_data = new_data;

        return true;
    }

    void init()
    {
        m_size = 0;
        m_max_size = Capacity;
        realloc();
    }

    bool grow()
    {
        m_max_size += Capacity;

        return realloc();
    }

    void construct_elements(size_t pos)
    {
        for( size_t i = pos; i < m_size; ++i )
        {
            Initializer<T>::construct( m_data + i, Token<std::is_pod<T>::value>() );    
        }
    }

    void destruct_elements( size_t pos )
    {
        for( size_t i = pos; i < m_size; ++i )
        {
            Initializer<T>::destruct( m_data + i, Token<std::is_pod<T>::value>() );    
        }
    }

    void copy_elements( const T* src, size_t pos )
    {
        for( size_t i = pos; i < m_size; ++i )
        {
            Initializer<T>::copy( m_data + i, src, Token<std::is_pod<T>::value>() );    
        }
    }
    
private:
    T* m_data;
    size_t m_size;
    size_t m_max_size;
};

} // namespace basic
