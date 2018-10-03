#include "engine.hpp"
#include "window.hpp"

Engine::Engine( )
    : m_window( IWindow::create( ) )
    , m_render( IRender::create( ) )
    , m_pool( )
    , m_quit( false )
{
}

void
Engine::add_drawable( IDrawable* drawable )
{
    m_init_drawables.push_back( drawable );
}

int
Engine::run( int width, int height )
{
    if( !m_window->init( width, height, "Hello" ) )
    {
        LOG("Failed init window.");

        return -1;
    }

    if( !m_render->init( width, width ) )
    {
        LOG("Failed init render.");

        return -1;
    }

    while ( !m_quit )
    {
        process_event( );

        if ( !m_init_drawables.empty( ) )
        {
            for ( auto dr : m_init_drawables )
            {
                dr->init( );
                m_drawables.push_back( dr );
            }

            m_init_drawables.clear( );
        }

        m_render->draw_begin( );

        for ( auto dr : m_drawables )
        {
            dr->draw( m_render.get( ) );
        }

        m_render->draw_end( m_window.get() );
    }

    return 0;
}

void
Engine::process_event( )
{
    if ( m_pool.is_need_update( ) )
    {
        m_pool.update( );
    }

    m_window->process_events( );

    m_quit = m_window->is_quit( );
}
