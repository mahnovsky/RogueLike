#include "thread_pool.hpp"
#include "worker.hpp"

#include <algorithm>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace async
{
ThreadPool::ThreadPool( )
    : m_workers( )
    , m_results( )
    , m_mutex( )
    , m_listeners( )
    , m_id_counter( 0 )
{
}

ThreadPool::~ThreadPool( )
{
    done_all( );
}

void
ThreadPool::done_all( )
{
    for ( auto& w : m_workers )
    {
        w->notify_done( );
    }
}

int
ThreadPool::add_listener( AsyncListener listener )
{
    if ( listener != nullptr )
    {
        const int id = m_id_counter++;

        m_listeners[ id ] = listener;

        return id;
    }

    return -1;
}

void
ThreadPool::add( std::unique_ptr< Worker >&& worker )
{
    m_workers.emplace_back( std::move( worker ) );
}

void
ThreadPool::run( WorkFunc func, AsyncListener listener )
{
    auto it = std::find_if(
            m_workers.begin( ), m_workers.end( ), []( const std::unique_ptr< Worker >& w ) {
                return !w->is_process( );
            } );

    const int id = add_listener( listener );

    if ( it != m_workers.end( ) )
    {
        ( *it )->notify( func, id );
    }
    else
    {
        std::unique_ptr< Worker > w = std::make_unique< Worker >( this );

        w->notify( func, id );

        add( std::move( w ) );
    }
}

void
ThreadPool::on_work_done( AsyncResultPtr& res, int id )
{
    std::unique_lock< std::mutex > lock( m_mutex );

    std::cout << "on work done" << std::endl;
    if ( res != nullptr )
    {
        res->set_id( id );

        m_results.emplace_back( std::move( res ) );
    }
}

void
ThreadPool::update( )
{
    if ( !m_results.empty( ) )
    {
        std::unique_lock< std::mutex > lock( m_mutex, std::try_to_lock );
        std::cout << "try on update, size: " << m_results.size( ) << std::endl;
        if ( !lock.owns_lock( ) )
        {
            return;
        }
        std::cout << "on update" << std::endl;

        for ( auto& res : m_results )
        {
            auto it = m_listeners.find( res->get_id( ) );

            if ( it != m_listeners.end( ) )
            {
                it->second( std::move( res ) );

                m_listeners.erase( it );
            }
        }

        m_results.clear( );
    }
}

bool
ThreadPool::is_need_update( )
{
    return !m_listeners.empty( );
}

class AsyncFileData : public IAsyncResult
{
public:
    AsyncFileData( char* data, size_t size )
        : m_data( data )
        , m_size( size )
    {
    }

    ~AsyncFileData( )
    {
        delete[] m_data;
        m_data = nullptr;
    }

    const char*
    get_data( ) const
    {
        return m_data;
    }

private:
    char* m_data;
    size_t m_size;
};

AsyncResultPtr
load_file( const char* path )
{
    FILE* f = fopen( path, "r" );
    if ( f )
    {
        fseek( f, 0, SEEK_END );
        const size_t len = ftell( f );
        fseek( f, 0, SEEK_SET );

        char* buf = new char[ len ];
        fread( buf, 1, len, f );

        fclose( f );

        return std::move( std::make_unique< AsyncFileData >( buf, len ) );
    }

    return nullptr;
}
}
/*
int
main( int argc, char** argv )
{
    ThreadPool pool;

    bool done1 = false;
    pool.run( []( ) -> AsyncResultPtr { return load_file( "test.cpp" ); },
              [&done1]( AsyncResultPtr&& res ) {
                  AsyncFileData* dat = dynamic_cast< AsyncFileData* >( res.get( ) );

                  if ( dat )
                  {
                      std::cout << dat->get_data( ) << std::endl;

                      done1 = true;
                  }
              } );

    bool done = false;

    pool.run( []( ) -> AsyncResultPtr { return load_file( "tree.cpp" ); },
              [&done]( AsyncResultPtr&& res ) {
                  AsyncFileData* dat = dynamic_cast< AsyncFileData* >( res.get( ) );
                  std::cout << "hello -----------" << std::endl;
                  if ( dat )
                  {
                      std::cout << dat->get_data( ) << std::endl;

                      done = true;
                  }
              } );

    try
    {
        while ( pool.is_need_update( ) )
        {
            pool.update( );
        }
    }
    catch ( ... )
    {
        std::cout << "---------- fail ---------" << std::endl;
        return 1;
    }
    std::cout << "done " << done << " done1 " << done1 << std::endl;

    pool.run( []( ) -> AsyncResultPtr { return load_file( "tree.cpp" ); },
              [&done]( AsyncResultPtr&& res ) {
                  AsyncFileData* dat = dynamic_cast< AsyncFileData* >( res.get( ) );
                  std::cout << "hello -----------" << std::endl;
                  if ( dat )
                  {
                      std::cout << dat->get_data( ) << std::endl;

                      done = false;
                  }
              } );

    while ( pool.is_need_update( ) )
    {
        pool.update( );
    }

    std::cout << "done " << done << " done1 " << done1 << std::endl;

    return 0;
}*/
