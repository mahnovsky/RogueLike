#pragma once

#include "basic/vector.hpp"

typedef void (*timer_function) (void*);

struct Timer
{
    Timer()
        :delay( 0.f )
        ,func( nullptr )
        ,user_data( nullptr )
        ,repeat_count( 0 )
        ,timestamp( 0 )
        ,is_removed( false )
    {}

    Timer( float delay, timer_function func, void* data, int repeat = 1 )
        :delay( delay )
        ,func( func )
        ,user_data( data )
        ,repeat_count( repeat )
        ,timestamp( 0 )
        ,is_removed( false )
    {}

    float delay;
    timer_function func;
    void* user_data;
    int repeat_count;
    double timestamp;
    bool is_removed;
};

class TimerManager
{
public:
    
    static TimerManager& get();

    void add( const Timer& timer );

    void add( float delay, timer_function func, void* user_data, int repeat = 1 );

    void update();

private:
    TimerManager();

    void remove_spant_timers();

private:
    basic::Vector<Timer> m_timers;
    basic::Vector<basic::uint32> m_remove_timers;
};
