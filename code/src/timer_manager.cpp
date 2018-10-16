#include "timer_manager.hpp"

static const int MAX_REMOVED_TIMERS = 16;

TimerManager& TimerManager::get()
{
    static TimerManager mng;

    return mng;
}

TimerManager::TimerManager()
    :m_timers()
{}

void TimerManager::add( const Timer& timer )
{
    ASSERT( timer.func != nullptr );

    size_t pos = m_timers.get_size();
    m_timers.push( timer );
    m_timers[pos].timestamp = basic::get_milliseconds(); 
}

void TimerManager::update( )
{
    basic::uint32 current = basic::get_milliseconds();

    for( size_t i = 0; i < m_timers.get_size(); ++i )
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
        for( size_t i = 0; i < m_remove_timers.get_size(); ++i )
        {
            size_t index = m_remove_timers[i];

            m_timers.remove_by_index( index );
        }
    }
}

