#include "basic/memory.hpp"
#include "basic/debug.hpp"
#include "basic/vector.hpp"

#include <malloc.h>
#include <math.h>
#include <cstring>

#define MAX_BUFFER_LEN 1024

namespace basic
{
template < class T >
static T*
ptr_plus( void* ptr, size_t offset )
{
    return reinterpret_cast< T* >( ( ( static_cast< char* >( ptr ) ) + offset ) );
}

template < class T >
static T*
ptr_minus( void* ptr, size_t offset )
{
    return reinterpret_cast< T* >( ( ( static_cast< char* >( ptr ) ) - offset ) );
}

struct InternalAllocator
{
    static uint32
    grow( uint32 capacity )
    {
        return capacity * 2;
    }

    static void*
    alloc_mem( void* ptr, memory_size bytes )
    {
        return realloc( ptr, bytes );
    }

    static void
    free_mem( void* ptr )
    {
        free( ptr );
    }
};

struct MemoryChunk
{
    memory_size chunk_size;
    uint32 index;
    const char* file;
    int line;
};

static MemoryChunk*
internal_get_chunk( void* ptr )
{
    return ptr_minus< MemoryChunk >( ptr, sizeof( MemoryChunk ) );
}

static void*
internal_get_user_ptr( MemoryChunk* ptr )
{
    return ptr_plus< MemoryChunk >( ptr, sizeof( MemoryChunk ) );
}

static memory_size
internal_get_user_mem_size( void* ptr )
{
    MemoryChunk* chunck = internal_get_chunk( ptr );

    return chunck->chunk_size - sizeof( MemoryChunk );
}

static mem_out_callback g_out_of_memory_callback;

static memory_size
get_mem_size( void* ptr )
{
    MemoryChunk* chunck = internal_get_chunk( ptr );

    return chunck->chunk_size;
}

static void*
internal_get_base_ptr( void* ptr )
{
    return ptr_minus< void >( ptr, sizeof( MemoryChunk ) );
}

static void*
internal_malloc( memory_size bytes, memory_size& real_size )
{
    const uint32 offset = sizeof( MemoryChunk );

    real_size = bytes + offset;

    void* res = malloc( real_size );
    if ( res )
    {
        MemoryChunk* chunk = static_cast< MemoryChunk* >( res );

        chunk->chunk_size = real_size;

        return ptr_plus< void >( res, offset );
    }

    return nullptr;
}

static void
internal_free( void* ptr, size_t& free_bytes )
{
    free_bytes = get_mem_size( ptr );

    void* base_ptr = internal_get_base_ptr( ptr );

    free( base_ptr );
}

static bool is_manager_alive = true;
class MemoryManager final
{
    struct FreeChunk
    {
        MemoryChunk* ptr;
        memory_size size;
    };

    enum
    {
        MIN_BLOCK_SIZE = 128,
        MAX_BLOCK_SIZE = 2048
    };
    static constexpr uint32 BUFFER_SIZE = 102400;
    struct Buffer
    {
        void* buff;
        uint32 size;
        uint32 offset;
    };

public:
    MemoryManager( )
        : m_chunks( )
        , m_memory_usage( 0 )
        , m_buffers( )
    {
        //  add_buffer( );
    }

    void
    add_buffer( )
    {
        Buffer b;
        b.buff = _mem_alloc( BUFFER_SIZE );
        b.size = BUFFER_SIZE;
        b.offset = 0;
        m_buffers.push( b );
    }

    ~MemoryManager( )
    {
        uint32 lost_memory = 0;
        for ( uint32 i = 0; i < m_chunks.get_size( ); ++i )
        {
            MemoryChunk* chunk = m_chunks[ i ];
            void* address = ptr_plus< void >( chunk, sizeof( MemoryChunk ) );

            LOG( "\tmemory leak %u bytes at address %x: %s: %d\n",
                 chunk->chunk_size,
                 address,
                 chunk->file,
                 chunk->line );

            lost_memory += chunk->chunk_size;

            free( chunk );
        }

        LOG( "total lost memory %u\n", lost_memory );
        is_manager_alive = false;
    }

    void
    add( MemoryChunk* chunk )
    {
        if ( !is_manager_alive )
            return;
        if ( chunk->index < m_chunks.get_size( ) && m_chunks[ chunk->index ] == chunk )
        {
            return;
        }

        chunk->index = m_chunks.get_size( );

        m_chunks.push( chunk );
    }

    void
    remove( MemoryChunk* chunk )
    {
        if ( !is_manager_alive )
            return;

        uint32 index = chunk->index;
        ASSERT( m_chunks[ index ] == chunk );
        if ( m_chunks.swap_remove( index ) )
        {
            m_chunks[ index ]->index = index;
        }
    }

    memory_size
    get_mem_usage( ) const
    {
        return m_memory_usage;
    }

    MemoryChunk*
    get_free_chunk( Vector< MemoryChunk*, InternalAllocator >& cont )
    {
        MemoryChunk* result = nullptr;
        if ( !cont.is_empty( ) )
        {
            result = cont.back( );
            cont.pop( );
        }

        return result;
    }

    uint32
    get_index( memory_size size, memory_size& need_size ) const
    {
        uint32 index = ceil( static_cast< float >( size ) / MIN_BLOCK_SIZE );
        need_size = index * MIN_BLOCK_SIZE;

        return index - 1;
    }

    void*
    alloc( memory_size size )
    {
        if ( !is_manager_alive || size == 0 )
        {
            return nullptr;
        }

        MemoryChunk* chunk = nullptr;
        memory_size need_size = size;

        if ( size <= MAX_BLOCK_SIZE )
        {
            uint32 index = get_index( size, need_size );

            chunk = get_free_chunk( m_free_stacks[ index ] );
        }

        if ( !chunk )
        {
            void* ptr = _mem_alloc( need_size );

            chunk = internal_get_chunk( ptr );

            m_memory_usage += chunk->chunk_size;
        }

        add( chunk );

        return internal_get_user_ptr( chunk );
    }

    void
    free_chunck( MemoryChunk* chunk )
    {
        if ( !is_manager_alive )
        {
            return;
        }

        uint32 user_size = chunk->chunk_size - sizeof( MemoryChunk );

        remove( chunk );

        if ( user_size <= MAX_BLOCK_SIZE )
        {
            memory_size nsize;
            uint32 index = get_index( user_size, nsize );

            m_free_stacks[ index ].push( chunk );
        }
        else
        {
            m_memory_usage -= chunk->chunk_size;
            free( chunk );
        }
    }

private:
    Vector< MemoryChunk*, InternalAllocator > m_chunks;

    Vector< MemoryChunk*, InternalAllocator > m_free_stacks[ 16 ];

    memory_size m_memory_usage;

    basic::Vector< Buffer, InternalAllocator > m_buffers;

} static memory_manager;

void*
_mem_alloc( memory_size bytes )
{
    if ( bytes == 0 )
    {
        return nullptr;
    }

    memory_size out_bytes = 0;
    void* res = internal_malloc( bytes, out_bytes );

    if ( !res && g_out_of_memory_callback )
    {
        g_out_of_memory_callback( );
    }

    return res;
}

void*
_mem_realloc( void* ptr, memory_size bytes )
{
    ASSERT( bytes > 0 );

    void* res = ptr;

    if ( ptr )
    {
        const memory_size mem_size = internal_get_user_mem_size( ptr );

        if ( bytes > mem_size )
        {
            res = memory_manager.alloc( bytes );

            mem_copy( res, ptr, mem_size );

            mem_free( ptr );
        }
    }
    else
    {
        res = memory_manager.alloc( bytes );
    }

    return res;
}

void
_mem_free( void* ptr )
{
    memory_manager.free_chunck( internal_get_chunk( ptr ) );
}

void*
_checked_mem_alloc( memory_size bytes, const char* file, int line )
{
    void* ptr = memory_manager.alloc( bytes );

    if ( ptr )
    {
        MemoryChunk* chunk = internal_get_chunk( ptr );
        chunk->file = file;
        chunk->line = line;
    }

    return ptr;
}

void*
_checked_mem_realloc( void* ptr, memory_size bytes, const char* file, int line )
{
    void* nptr = _mem_realloc( ptr, bytes );

    if ( nptr != ptr )
    {
        MemoryChunk* chunk = ptr_minus< MemoryChunk >( nptr, sizeof( MemoryChunk ) );

        chunk->file = file;
        chunk->line = line;
    }

    return nptr;
}

void*
mem_move( void* destination, const void* source, memory_size bytes )
{
    ASSERT( destination != source );
    ASSERT( source != nullptr );
    ASSERT( bytes > 0 );

    return memmove( destination, source, bytes );
}

void*
mem_copy( void* destination, const void* source, memory_size bytes )
{
    ASSERT( destination != source );
    ASSERT( source != nullptr );
    ASSERT( bytes > 0 );

    return memcpy( destination, source, bytes );
}

int
mem_cmp( const void* ptr1, const void* ptr2, memory_size bytes )
{
    ASSERT( ptr1 != ptr2 );
    ASSERT( ptr2 != nullptr );
    ASSERT( bytes > 0 );

    return memcmp( ptr1, ptr2, bytes );
}

void
mem_set_out_of_memory( mem_out_callback callback )
{
    ASSERT( callback != nullptr );

    g_out_of_memory_callback = callback;
}

size_t
get_memory_usage( )
{
    return memory_manager.get_mem_usage( );
}

void*
wrapper_malloc( size_t size )
{
    return malloc( size );
}

void*
wrapper_realloc( void* ptr, size_t size )
{
    return realloc( ptr, size );
}

void
wrapper_free( void* ptr )
{
    free( ptr );
}
}

void*
operator new( std::size_t n )
{
    return basic::mem_alloc( static_cast< basic::memory_size >( n ) );
}

void
operator delete( void* p ) noexcept
{
    basic::mem_free( p );
}

void*
operator new[]( std::size_t s )
{
    return basic::mem_alloc( static_cast< basic::memory_size >( s ) );
}

void
operator delete[]( void* p ) noexcept
{
    basic::mem_free( p );
}
