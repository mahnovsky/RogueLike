#pragma once

#include "pair.hpp"
#include "string.hpp"
#include "vector.hpp"

namespace basic
{
template < typename K >
struct HashFn
{
    static uint32
    hash( const K& key )
    {
        return 0;
    }
};

template <>
struct HashFn< uint32 >
{
    static uint32
    hash( const uint32& in )
    {
        uint32 x = in;
        x = ( ( x >> 16 ) ^ x ) * 0x45d9f3b;
        x = ( ( x >> 16 ) ^ x ) * 0x45d9f3b;
        x = ( x >> 16 ) ^ x;
        return x;
    }
};

template <>
struct HashFn< int32 >
{
    static uint32
    hash( const int32& in )
    {
        return HashFn< uint32 >::hash( static_cast< uint32 >( in ) );
    }
};

template <>
struct HashFn< String >
{
    static uint32
    hash( const String& in )
    {
        uint32 res = 0;
        for ( uint32 i = 0; i < in.get_size( ); ++i )
        {
            uint32 ch = HashFn< uint32 >::hash( in[ i ] );
            res += ch;
        }
        return res;
    }
};

template < typename T >
class Iterator
{
public:
    Iterator( T& t )
        : m_it( t.get_iterator( ) )
    {
    }

    operator bool( ) const
    {
        return m_it;
    }

private:
    typename T::Iterator m_it;
};

template < typename K, typename V, typename HASH_FN = HashFn< K > >
class HashMap
{
    using InternalPair = Pair< K, V >;

    struct Bucket
    {
        static constexpr uint32 MAX_ELEMENTS = 4;
        uint32 count = 0;
        InternalPair elements[ MAX_ELEMENTS ];

        uint32
        find_index( const K& key ) const
        {
            for ( uint32 i = 0; i < count; ++i )
            {
                if ( key == elements[ i ].key )
                {
                    return i;
                }
            }
            return MAX_ELEMENTS;
        }

        bool
        insert( InternalPair& p )
        {
            uint32 index = find_index( p.key );
            bool result = ( index + 1 ) < MAX_ELEMENTS;

            if ( result )
            {
                elements[ index ] = std::move( p );
            }

            if ( !result && ( count + 1 ) < MAX_ELEMENTS )
            {
                elements[ count++ ] = std::move( p );
                result = true;
            }

            return result;
        }

        void
        remove( uint32 index )
        {
            uint32 last = count - 1;
            if ( index < last )
            {
                elements[ index ] = elements[ last ];
            }

            --count;
        }

        bool
        find_by_index( uint32 index, Pair< K, V >** out )
        {
            for ( uint32 i = 0; i < count; ++i )
            {
                if ( elements[ i ].index == index )
                {
                    ( *out ) = &elements[ i ].p;
                    return true;
                }
            }
            return false;
        }
    };

    static basic::Vector< Bucket >
    rebuild( basic::Vector< Bucket >& table )
    {
        basic::Vector< Bucket > new_table;
        basic::uint32 len = table.get_size( );
        new_table.resize( len * 2 );
        uint32 size = 0;
        for ( uint32 i = 0; i < table.get_size( ); ++i )
        {
            Bucket& b = table[ i ];
            for ( uint32 j = 0; j < b.count; ++j )
            {
                internal_insert( new_table, size, std::move( b.elements[ j ] ) );
            }
        }

        return new_table;
    }

    static void
    internal_insert( basic::Vector< Bucket >& table, uint32& out_size, InternalPair&& p )
    {
        uint32 pos = get_pos( p.key, table.get_size( ) );

        if ( table[ pos ].insert( p ) )
        {
            ++out_size;
        }
        else
        {
            table = std::move( rebuild( table ) );
            internal_insert( table, out_size, std::move( p ) );
        }
    }

    static uint32
    get_pos( const K& key, uint32 table_len )
    {
        uint32 hash_val = HASH_FN::hash( key );
        uint32 pos = hash_val % table_len;

        return pos;
    }

public:
    using Type = InternalPair;

    HashMap( )
        : m_size( 0 )
        , m_table( )
    {
        m_table.resize( 10 );
    }

    uint32
    get_size( ) const
    {
        return m_size;
    }

    void
    insert( const K& key, const V& val )
    {
        InternalPair p{key, val};
        internal_insert( m_table, m_size, std::move( p ) );
    }

    bool
    find( const K& key, V& out )
    {
        uint32 pos = get_pos( key, m_table.get_size( ) );

        Bucket& b = m_table[ pos ];
        uint32 index = b.find_index( key );

        if ( index != Bucket::MAX_ELEMENTS )
        {
            out = b.elements[ index ].value;

            return true;
        }

        return false;
    }

    bool
    contains( const K& key )
    {
        uint32 pos = get_pos( key, m_table.get_size( ) );

        Bucket& b = m_table[ pos ];
        uint32 index = b.find_index( key );

        return index != Bucket::MAX_ELEMENTS;
    }

    void
    remove( const K& key )
    {
        uint32 pos = get_pos( key, m_table.get_size( ) );

        Bucket& b = m_table[ pos ];
        uint32 index = b.find_index( key );

        if ( index != Bucket::MAX_ELEMENTS )
        {
            b.remove( index );
            --m_size;
        }
    }

public:
    class Iterator
    {
    public:
        Iterator( const HashMap< K, V >& map )
            : m_map( map )
            , m_bucket( 0 )
            , m_index( 0 )
        {
        }

        Iterator( const HashMap< K, V >& map, uint32 bucket, uint32 index )
            : m_map( map )
            , m_bucket( bucket )
            , m_index( index )
        {
        }

        ~Iterator( ) = default;

        void
        reset( )
        {
            m_bucket = 0;
            m_index = 0;
        }

        bool
        has_next( ) const
        {
            uint32 bp = m_bucket;
            const uint32 bs = m_map.m_table.get_size( );
            const Bucket* current = &m_map.m_table[ bp ];

            do
            {
                if ( current->count > 0 )
                {
                    return true;
                }

                current = &m_map.m_table[ ++bp ];
            } while ( bp < bs );

            return false;
        }

        operator bool( ) const
        {
            return has_next( );
        }

        const V&
        value( ) const
        {
            const Bucket* current = &m_map.m_table[ m_bucket ];

            return current->elements[ m_index ].value;
        }

        bool
        is_valid( ) const
        {
            return m_bucket < m_map.m_table.get_size( );
        }

        Iterator& operator++( )
        {
            const Bucket* current = &m_map.m_table[ m_bucket ];
            const uint32 bs = m_map.m_table.get_size( );

            ++m_index;
            while ( current->count <= m_index )
            {
                m_index = 0;
                ++m_bucket;
                if ( m_bucket >= bs )
                {
                    return *this;
                }

                current = &m_map.m_table[ m_bucket ];
            }

            return *this;
        }

        const InternalPair& operator*( ) const
        {
            return m_map.m_table[ m_bucket ].elements[ m_index ];
        }

        bool
        operator==( const Iterator& it ) const
        {
            return m_bucket == it.m_bucket && m_index == it.m_index;
        }

        bool
        operator!=( const Iterator& it ) const
        {
            return !( it == *this );
        }

        const InternalPair* operator->( ) const
        {
            return &m_map.m_table[ m_bucket ].elements[ m_index ];
        }

    private:
        const HashMap< K, V >& m_map;
        mutable uint32 m_bucket;
        mutable uint32 m_index;
    };

    Iterator
    find( const K& key ) const
    {
        uint32 pos = get_pos( key, m_table.get_size( ) );

        const Bucket& b = m_table[ pos ];
        uint32 index = b.find_index( key );

        if ( index != Bucket::MAX_ELEMENTS )
        {
            return Iterator( *this, pos, index );
        }

        return end( );
    }

    Iterator
    get_iterator( )
    {
        return Iterator( *this );
    }

    Iterator
    begin( ) const
    {
        uint32 b = 0;
        for ( uint32 i = 0; i < m_table.get_size( ); ++i )
        {
            if ( m_table[ i ].count > 0 )
            {
                b = i;
                break;
            }
        }
        return Iterator( *this, b, 0 );
    }

    Iterator
    end( ) const
    {
        return Iterator( *this, m_table.get_size( ), 0 );
    }

    friend class Iterator;

private:
    uint32 m_size;
    Vector< Bucket > m_table;
};
}
