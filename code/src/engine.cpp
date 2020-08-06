#include "engine.hpp"
#include "window.hpp"
#include "basic/debug.hpp"
#include "basic/string.hpp"
#include "basic/time.hpp"
#include "timer_manager.hpp"

#include "render.hpp"
#include "window.hpp"

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
    , m_input(nullptr)
    , m_quit( false )
    , m_callbacks()
    , m_cmd_args()
    , m_time( 0.0 )
    , m_delta( 0.0 )
    , m_fps( 0 )
    , m_object_manager(NEW_OBJ(GenericObjectManager))
    , m_ecs(NEW_OBJ(EcsManager, m_object_manager))
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

Engine::~Engine()
{
	DELETE_OBJ(m_ecs);
	DELETE_OBJ(m_object_manager);
}

bool Engine::init(int width, int height, const char *wnd_title)
{
    m_window = IWindow::create();
    if( !m_window->init( width, height, wnd_title ) )
    {
        LOG("Failed init window.");

        return false;
    }

    auto storage = m_ecs->add_system<ResourceStorage>();
    m_render = IRender::create();
    if( !m_render->init( storage, width, height ) )
    {
        LOG("Failed init render.");

        return false;
    }

    m_input = input::Input::create();

    if( m_callbacks[ Init ])
    {
        m_callbacks[ Init ]( this );
    }

    LOG( "Engine initialization done. Memory usage %lu",
        basic::get_memory_usage() );

    return true;
}

bool Engine::update()
{
    static basic::uint32 fps_counter;

    double begin = basic::get_milliseconds();

    process_event( );

    m_callbacks[ Frame ]( this );

    TimerManager::get().update();

    m_render->clear();

    m_render->present();

	m_callbacks[Draw](this);

	m_window->swap_buffers();

    m_delta = basic::get_milliseconds() - begin;
    m_time += m_delta;

    ++fps_counter;
    if( m_time >= 1000.0 )
    {
        m_fps = fps_counter;
        m_time = 0.0;
        fps_counter = 0;
    }

    return !m_quit;
}

void Engine::cleanup()
{
    if( m_callbacks[ Clean ] )
    {
        m_callbacks[ Clean ]( this );
    }

    DELETE_OBJ(m_input);
    m_input = nullptr;
    DELETE_OBJ(m_render);
    m_render = nullptr;
    DELETE_OBJ(m_window);
    m_window = nullptr;

    LOG( "Engine free done. Memory usage %lu",
        basic::get_memory_usage() );
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

void Engine::shutdown()
{
    m_quit = true;
}

IRender* Engine::get_render()
{
    return m_render;
}

input::Input *Engine::get_input()
{
    return  m_input;
}

glm::vec2 Engine::get_window_size() const
{
    int x, y;
    m_window->get_size( x, y );

    return glm::vec2(x, y);
}

double Engine::get_frame_time() const
{
    return m_delta;
}

basic::uint32 Engine::get_fps() const
{
    return m_fps;
}

void
Engine::process_event( )
{
    m_window->process_events( m_input );

    if( m_window->is_quit() )
    {
        m_quit = true;
    }
}

