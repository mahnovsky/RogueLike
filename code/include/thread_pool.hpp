#pragma once

#include "async_common.hpp"

namespace async
{
class ThreadPool
{
public:
    ThreadPool( );
    ~ThreadPool( );

    void done_all( );

    int add_listener( AsyncListener listener );

    void add( std::unique_ptr< Worker >&& worker );

    void run( WorkFunc func, AsyncListener listener = nullptr );

    void on_work_done( AsyncResultPtr& res, int id );

    void update( );

    bool is_need_update( );

private:
    std::vector< std::unique_ptr< Worker > > m_workers;
    std::vector< std::unique_ptr< IAsyncResult > > m_results;
    std::mutex m_mutex;

    std::map< int, AsyncListener > m_listeners;
    int m_id_counter;
};
}
