#include "timer_manager.hpp"
#include "basic/time.hpp"

static const int MAX_REMOVED_TIMERS = 64;

TimerManager& TimerManager::get()
{
    static TimerManager mng;

    return mng;
}

TimerManager::TimerManager()
    :m_timers()
    ,m_remove_timers()
{}

void TimerManager::add( const Timer& timer )
{
    ASSERT( timer.func != nullptr );

    if( !m_remove_timers.is_empty() )
    {
        basic::uint32 pos = m_remove_timers.back();
 
        ASSERT( pos < m_timers.get_size() );
        ASSERT( m_timers[pos].is_removed == true );

        m_remove_timers.pop();

        m_timers[pos] = timer;
        m_timers[pos].timestamp = basic::get_milliseconds();
    }
    else
    {
        basic::uint32 pos = m_timers.get_size();
        m_timers.push( timer );
        m_timers[pos].timestamp = basic::get_milliseconds();
    }
}

void TimerManager::add(float delay, timer_function func, void *user_data, int repeat)
{
    Timer t;
    t.func = func;
    t.delay = delay;
    t.user_data = user_data;
    t.repeat_count = repeat;

    add( t );
}

void TimerManager::update( )
{
    double current = basic::get_milliseconds();

    for( basic::uint32 i = 0; i < m_timers.get_size(); ++i )
    {
        Timer& timer = m_timers[i];

        float delta = static_cast<float>( current - timer.timestamp ) / 1000;

        if( !timer.is_removed && delta > timer.delay )
        {
            timer.func( timer.user_data );
            
            if( timer.repeat_count > 0 )
            {
                --timer.repeat_count;
            }

            if( timer.repeat_count == 0 )
            {
                m_remove_timers.push( i ); 
                timer.is_removed = true;

                continue;
            }

            timer.timestamp = basic::get_milliseconds();
        }
    }

    remove_spant_timers();
}

void TimerManager::remove_spant_timers()
{
    if( !m_remove_timers.is_empty() && 
        m_remove_timers.get_size() > MAX_REMOVED_TIMERS )
    {
        for( basic::uint32 i = 0; i < m_remove_timers.get_size(); ++i )
        {
            basic::uint32 index = m_remove_timers[i];

            m_timers.remove_by_index( index );
        }

        m_remove_timers.clear();
    }
}

