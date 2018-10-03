#pragma once

#include <condition_variable>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

namespace async
{
class IAsyncResult
{
public:
    virtual ~IAsyncResult( )
    {
    }

    void
    set_id( int id )
    {
        m_id = id;
    }

    int
    get_id( ) const
    {
        return m_id;
    }

private:
    int m_id;
};

class Worker;
class ThreadPool;

using AsyncResultPtr = std::unique_ptr< IAsyncResult >;
using AsyncListener = std::function< void( AsyncResultPtr&& ) >;
using WorkFunc = std::function< AsyncResultPtr( ) >;
}
