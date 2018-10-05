#include "engine.hpp"
#include "window.hpp"
#include "basic/util_functions.hpp"

Engine* Engine::_instance;

void Engine::out_of_memory()
{
    if( _instance->m_callbacks[ OutOfMemory ] )
    {
        _instance->m_callbacks[ OutOfMemory ]( _instance );
    }
    _instance->m_quit = true; 
}

static void dummy( Engine* ){}

Engine::Engine( int argv, char** argc )
    : m_window( IWindow::create( ) )
    , m_render( IRender::create( ) )
    , m_quit( false )
    , m_callbacks()
{
    ASSERT_M( _instance == nullptr, "Only one instance of Engine can be exist" );

    _instance = this;
    basic::mem_set_out_of_memory( &Engine::out_of_memory );

    m_callbacks[Frame] = &dummy;
    m_callbacks[Draw] = &dummy;
}

IRender* Engine::get_render()
{
    return m_render;
}

void Engine::set_callback( EngineCallbackType type, engine_callback callback )
{
    const size_t index = static_cast<size_t>( type );
    if( m_callbacks[ index ] )
    {
        LOG( "callback type: %d override", index );
    }

    m_callbacks[ index ] = callback;
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

    if( m_callbacks[ Init ])
    {
        m_callbacks[ Init ]( this );
    }

    while ( !m_quit )
    {
        process_event( );

        m_callbacks[ Frame ]( this );
        
        m_render->draw_begin( );

        m_callbacks[ Draw ]( this );

        m_render->draw_end( m_window );
    }

    if( m_callbacks[ Clean ] )
    {
        m_callbacks[ Clean ]( this );
    }

    delete m_render;
    m_render = nullptr;
    delete m_window;
    m_render = nullptr;

    return 0;
}

void
Engine::process_event( )
{
    m_window->process_events( );

    m_quit = m_window->is_quit( );
}
