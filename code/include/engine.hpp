#pragma once

#include "render.hpp"
#include "thread_pool.hpp"
#include "window.hpp"

class IDrawable
{
public:
    virtual void init( ) = 0;

    virtual void draw( IRender* render ) = 0;
};

class Engine
{
public:
    Engine( );

    void add_drawable( IDrawable* drawable );

    int run( int width, int height );

private:
    void process_event( );

private:
    std::unique_ptr< IWindow > m_window;

    std::unique_ptr< IRender > m_render;

    async::ThreadPool m_pool;

    bool m_quit;

    std::vector< IDrawable* > m_init_drawables;
    std::vector< IDrawable* > m_drawables;
};
