#include "worker.hpp"
#include "thread_pool.hpp"

namespace async
{
Worker::Worker( ThreadPool* pool )
    : m_thread( &Worker::func, this )
    , m_done( false )
    , m_noticed( false )
    , m_condition( )
    , m_mutex( )
    , m_pool( pool )
    , m_id( -1 )
{
    std::cout << "on created worker" << std::endl;
}

Worker::~Worker( )
{
    m_thread.join( );
}

void
Worker::notify( WorkFunc func, int id )
{
    std::unique_lock< std::mutex > lock( m_mutex );

    m_id = id;
    m_work_func = func;
    m_noticed = true;
    m_condition.notify_one( );
}

bool
Worker::is_process( ) const
{
    return m_work_func != nullptr;
}

void
Worker::notify_done( )
{
    std::unique_lock< std::mutex > lock( m_mutex );
    m_id = -1;
    m_done = true;
    m_noticed = true;
    m_condition.notify_one( );
}

void
Worker::func( )
{
    while ( !m_done )
    {
        std::unique_lock< std::mutex > lock( m_mutex );
        while ( !m_noticed )
        {
            m_condition.wait( lock );
        }

        m_noticed = false;
        if ( m_done )
        {
            break;
        }

        if ( m_work_func != nullptr )
        {
            AsyncResultPtr res = m_work_func( );
            std::cout << "work done " << m_id << std::endl;
            if ( m_id >= 0 )
            {
                m_pool->on_work_done( res, m_id );
            }

            m_id = -1;
            m_work_func = nullptr;
        }
    }
}
}
