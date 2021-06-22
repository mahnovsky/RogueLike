#include "engine.hpp"
#include "window.hpp"
#include "debug.hpp"
#include "string.hpp"
#include "time.hpp"
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
    _instance->m_is_runned = true; 
}

static void dummy( IEngine* ){}

Engine::Engine( int argc, char** argv )
    : m_system_manager()
	, m_window( nullptr ) 
    , m_render( nullptr )
    , m_input(nullptr)
    , m_is_runned( true )
    , m_callbacks()
    , m_cmd_args()
    , m_time( 0.0 )
    , m_delta( 0.0 )
    , m_fps( 0 )
    , m_ecs(NEW_OBJ(EntityComponentManager))
	, IEngine(m_system_manager)
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

	m_system_manager.add_system(this);
}

Engine::~Engine()
{
	DELETE_OBJ(m_ecs);
}

bool Engine::init(int width, int height, const char *wnd_title)
{
	m_input = input::Input::create();

    m_window = IWindow::create();
    if( !m_window->init( width, height, wnd_title ) )
    {
        LOG("Failed init window.");

        return false;
    }

    m_render = IRender::create();
	core::ResourceStorage* rs = m_system_manager.get_system<core::ResourceStorage>();

    if( !m_render->init( rs, width, height ) )
    {
        LOG("Failed init render.");

        return false;
    }

	m_system_manager.initialize();

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

    return m_is_runned;
}

void Engine::cleanup()
{
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
    m_is_runned = false;
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

bool Engine::is_runned() const
{
    return m_is_runned;
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
		m_system_manager.shutdown();
    }
}

