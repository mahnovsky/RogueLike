#include "basic/memory.hpp"
#include "basic/debug.hpp"
#include "basic/vector.hpp"

#include <cstring>
#include <malloc.h>

#define MAX_BUFFER_LEN 1024

namespace basic
{

template <class T>
static T* ptr_plus(void* ptr, size_t offset)
{
    return reinterpret_cast<T*>( ( (static_cast<char*>(ptr)) + offset ) );
}

template <class T>
static T* ptr_minus(void* ptr, size_t offset)
{
    return reinterpret_cast<T*>( ( (static_cast<char*>(ptr)) - offset ) );
}

struct InternalAllocator
{
    static uint32 grow( uint32 capacity ) { return capacity * 2; }

    static void* alloc_mem( void* ptr, memory_size bytes ) { return realloc( ptr, bytes ); }

    static void free_mem( void* ptr ) { free( ptr ); }
};

struct MemoryChunk
{
    memory_size chunk_size;
    ref_count refs;

#ifdef _DEBUG
    uint32 index;
    const char* file;
    int line;
#endif
};

#ifdef _DEBUG
class MemoryManager final
{
public:
    MemoryManager()
        :m_chunks()
        ,m_memory_usage(0)
    {}

    ~MemoryManager()
    {
        printf("lost memory %u\n", m_memory_usage);

        for(uint32 i = 0; i < m_chunks.get_size(); ++i)
        {
            MemoryChunk* chunk = m_chunks[i];
            void* address = ptr_plus<void>( chunk, sizeof (MemoryChunk));
            printf( "\tmemory leak %u bytes at address %x: %s: %d\n", chunk->chunk_size, address,
                    chunk->file, chunk->line );

            free( chunk );
        }
    }

    void add( MemoryChunk* chunk )
    {
        if( chunk->index < m_chunks.get_size() &&
            m_chunks[chunk->index] == chunk )
        {
            return;
        }

        m_memory_usage += chunk->chunk_size;
        chunk->index = m_chunks.get_size();

        m_chunks.push( chunk );
    }

    void remove( MemoryChunk* chunk )
    {
        m_memory_usage -= chunk->chunk_size;

        if( chunk == m_chunks.back() )
        {
            m_chunks.pop();
            return;
        }

        uint32 index = chunk->index;
        m_chunks.swap_remove( index );

        m_chunks[index]->index = index;
    }

    memory_size get_mem_usage() const
    {
        return m_memory_usage;
    }

private:
    Vector< MemoryChunk*, InternalAllocator > m_chunks;
    memory_size m_memory_usage;
}
static memory_manager;
#endif

static mem_out_callback g_out_of_memory_callback;
static memory_size memory_usage;


static MemoryChunk* internal_get_chunk( void* ptr )
{
    return ptr_minus<MemoryChunk>( ptr, sizeof (MemoryChunk) );
}

static memory_size get_mem_size(void* ptr)
{
    MemoryChunk* chunck = internal_get_chunk( ptr );

    return chunck->chunk_size;
}

static memory_size get_user_mem_size(void* ptr)
{
    MemoryChunk* chunck = internal_get_chunk( ptr );

    return chunck->chunk_size - sizeof (MemoryChunk);
}

static void* internal_get_base_ptr(void* ptr)
{
    return ptr_minus<void>( ptr, sizeof( MemoryChunk ) );
}

static void* internal_malloc(memory_size bytes, memory_size& real_size)
{
    const uint32 offset = sizeof( MemoryChunk );

    real_size = bytes + offset;

    void* res = malloc( real_size );
    if (res)
    {
        MemoryChunk* chunk = static_cast<MemoryChunk*>(res);

        chunk->chunk_size = real_size;
        chunk->refs = 0;

        return ptr_plus<void>(res, offset);
    }

    return nullptr;
}

static void internal_free(void* ptr, size_t& free_bytes )
{
    free_bytes = get_mem_size( ptr );

    ASSERT( get_refs(ptr) == 0 );

    void* base_ptr = internal_get_base_ptr(ptr);

    free( base_ptr );
}

void*
_mem_alloc(memory_size bytes )
{
    if(bytes == 0)
    {
        return nullptr;
    }

    memory_size out_bytes = 0;
    void* res = internal_malloc( bytes, out_bytes );

    if ( !res && g_out_of_memory_callback )
    {
        g_out_of_memory_callback( );
    }

    memory_usage += out_bytes;

    return res;
}


void*
_mem_realloc( void* ptr, memory_size bytes )
{
    ASSERT( bytes > 0 );

    void* res = ptr;

    if ( ptr )
    {
        const memory_size mem_size = get_user_mem_size(ptr);

        if (bytes > mem_size)
        {
            res = _mem_alloc(bytes);

            mem_copy(res, ptr, mem_size);

            mem_free(ptr);
        }
    }
    else
    {
        res = _mem_alloc(bytes);
    }

    return res;
}

void mem_free( void* ptr )
{
#ifdef _DEBUG
   memory_manager.remove( internal_get_chunk( ptr ) );
#endif

    size_t free_bytes = 0;

    internal_free( ptr, free_bytes );

    memory_usage -= free_bytes;
}

void *_checked_mem_alloc( memory_size bytes, const char *file, int line )
{
    void* ptr = _mem_alloc( bytes );
#ifdef _DEBUG
    if( ptr )
    {
        MemoryChunk* chunk = ptr_minus< MemoryChunk >( ptr, sizeof(MemoryChunk) );
        chunk->index = 0;
        chunk->file = file;
        chunk->line = line;

        memory_manager.add( chunk );
    }
#endif
    return ptr;
}

void *_checked_mem_realloc( void* ptr, memory_size bytes, const char* file, int line )
{
    void* nptr = _mem_realloc( ptr, bytes );
#ifdef _DEBUG
    if( nptr != ptr )
    {
        MemoryChunk* chunk = ptr_minus< MemoryChunk >( nptr, sizeof(MemoryChunk) );
        chunk->index = 0;
        chunk->file = file;
        chunk->line = line;

        memory_manager.add( chunk );
    }
#endif
    return nptr;
}

void*
mem_move(void* destination, const void* source, memory_size bytes )
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
mem_cmp(const void* ptr1, const void* ptr2, memory_size bytes )
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

size_t get_memory_usage()
{
    return memory_usage;
}

ref_count increment_ref(void *ptr)
{
    MemoryChunk* chunk = internal_get_chunk( ptr );

    return ++chunk->refs;
}

ref_count decrement_ref(void *ptr)
{
    MemoryChunk* chunk = internal_get_chunk( ptr );

    return --chunk->refs;
}

ref_count get_refs(void *ptr)
{
    MemoryChunk* chunk = internal_get_chunk( ptr );

    return chunk->refs;
}

}

void*
operator new( std::size_t n )
{
    return basic::mem_alloc( static_cast<basic::memory_size>(n) );
}

void
operator delete( void* p ) noexcept
{
    basic::mem_free( p );
}

void*
operator new[]( std::size_t s )
{
    return basic::mem_alloc( static_cast<basic::memory_size>(s) );
}

void
operator delete[]( void* p ) noexcept
{
    basic::mem_free( p );
}
