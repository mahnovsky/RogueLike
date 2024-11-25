#include "engine.hpp"
#include "window.hpp"
#include "debug.hpp"
#include "string.hpp"
#include "time.hpp"
#include "timer_manager.hpp"

#include "render.hpp"
#include "window.hpp"

#include "iglobal_context.hpp"

Engine* Engine::_instance;

void Engine::out_of_memory()
{
    if( _instance->m_callbacks[ OutOfMemory ] )
    {
        _instance->m_callbacks[ OutOfMemory ]( _instance );
    }
    _instance->m_is_runned = false;
}

void Engine::update_fps()
{
	static basic::uint32 fps_counter;

	m_fps_time += m_frame_time;
	++fps_counter;

	if (m_fps_time >= 1000.0)
	{
		m_fps = fps_counter;
		m_fps_time = 0.0;
		fps_counter = 0;
	}
}

static void dummy( IEngine* ){}

Engine::Engine( core::IGlobalContext* context )
    : _context(context)
    , m_window( nullptr ) 
    , m_render( nullptr )
    , m_input(nullptr)
    , m_is_runned( true )
    , m_callbacks()
    , m_cmd_args()
    , m_fps_time( 0.0 )
    , m_frame_time( 0.0 )
    , m_fps( 0 )
    , m_ecs(NEW_OBJ(EntityComponentManager))
{
    ASSERT_M( _instance == nullptr, "Only one instance of Engine can be exist" );

    _instance = this;
    basic::mem_set_out_of_memory( &Engine::out_of_memory );

    m_callbacks[Frame] = &dummy;
    m_callbacks[Draw] = &dummy;
}

bool Engine::initialize(int width, int height, const char *wnd_title)
{
	m_input = input::Input::create();

    m_window = IWindow::create();
    if( !m_window->init( width, height, wnd_title ) )
    {
        LOG("Failed init window.");

        return false;
    }

    m_render = IRender::create();

    core::SystemManager* systems = _context->get_system_manager();
	core::ResourceStorage* rs = systems->get_system<core::ResourceStorage>();

    if( !m_render->init( rs, width, height ) )
    {
        LOG("Failed init render.");

        return false;
    }

	systems->update();

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
    const double frame_begin = basic::get_milliseconds();

    process_event( );

    m_callbacks[ Frame ]( this );

    TimerManager::get().update();

    m_render->clear();

    m_render->present();

	m_callbacks[Draw](this);

	m_window->swap_buffers();

    m_frame_time = basic::get_milliseconds() - frame_begin;
    
    update_fps();

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
    DELETE_OBJ(m_ecs);
    
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
    return m_frame_time;
}

bool Engine::is_runned() const
{
    return m_is_runned;
}

basic::uint32 Engine::get_fps() const
{
    return m_fps;
}

void Engine::process_event( )
{
    m_window->process_events( m_input );

    if( m_window->is_quit() )
    {
        m_is_runned = false;
    }
}

