#pragma once

#include "async_common.hpp"

namespace async
{
class Worker
{
public:
    Worker( ThreadPool* pool );

    ~Worker( );

    void func( );

    void notify( WorkFunc func, int id = -1 );

    bool is_process( ) const;

    void notify_done( );

private:
    std::thread m_thread;
    bool m_done;
    bool m_noticed;
    std::condition_variable m_condition;
    std::mutex m_mutex;
    WorkFunc m_work_func;
    ThreadPool* m_pool;
    int m_id;
};
}
