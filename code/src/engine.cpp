#include "engine.hpp"
#include "window.hpp"
#include "basic/debug.hpp"
#include "basic/string.hpp"
#include "basic/time.hpp"
#include "timer_manager.hpp"

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

Engine::Engine( int argc, char** argv )
    : m_window( nullptr ) 
    , m_render( nullptr )
    , m_quit( false )
    , m_callbacks()
    , m_cmd_args()
    , m_time( 0.0 )
    , m_delta( 0.0 )
    , m_fps( 0 )
{
    ASSERT_M( _instance == nullptr, "Only one instance of Engine can be exist" );

    _instance = this;
    basic::mem_set_out_of_memory( &Engine::out_of_memory );

    m_callbacks[Frame] = &dummy;
    m_callbacks[Draw] = &dummy;

    basic::uint32 uargc = argc < 0 ? 0 : static_cast<basic::uint32>(argc);
    for( basic::uint32 i = 0; i < uargc; ++i )
    {
       m_cmd_args.emplace( argv[i] );
    }
}

IRender* Engine::get_render()
{
    return m_render;
}

double Engine::get_frame_time() const
{
    return m_delta;
}

basic::uint32 Engine::get_fps() const
{
    return m_fps;
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

void Engine::get_window_size(int& out_width, int& out_height)
{
    m_window->get_size( out_width, out_height );
}

int
Engine::run( int width, int height, const char* wnd_title )
{
    m_window = IWindow::create();
    if( !m_window->init( width, height, wnd_title ) )
    {
        LOG("Failed init window.");

        return -1;
    }

    m_render = IRender::create();
    if( !m_render->init( width, height ) )
    {
        LOG("Failed init render.");

        return -1;
    }

    if( m_callbacks[ Init ])
    {
        m_callbacks[ Init ]( this );
    }

    LOG( "Engine initialization done. Memory usage %lu",
        basic::get_memory_usage() );


    static basic::uint32 fps_counter;
    while ( !m_quit )
    {
        double begin = basic::get_milliseconds();

        process_event( );

        if( m_window->is_quit() )
        {
            break;
        }        

        m_callbacks[ Frame ]( this );

        TimerManager::get().update();
        
        m_render->draw_begin( );

        m_callbacks[ Draw ]( this );

        m_render->draw_end( m_window );

        m_delta = basic::get_milliseconds() - begin;
        m_time += m_delta;

        ++fps_counter;
        if( m_time >= 1000.0 )
        {
            m_fps = fps_counter;
            m_time = 0.0;
            fps_counter = 0;
        }
    }

    if( m_callbacks[ Clean ] )
    {
        m_callbacks[ Clean ]( this );
    }

    delete m_render;
    m_render = nullptr;
    delete m_window;
    m_render = nullptr;

    LOG( "Engine free done. Memory usage %lu",
        basic::get_memory_usage() );

    return 0;
}

void
Engine::process_event( )
{
    m_window->process_events( );

    m_quit = m_window->is_quit( );
}
