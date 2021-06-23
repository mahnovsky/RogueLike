#include "engine.hpp"
#include "game/game_instance.hpp"
#include "sprite.hpp"
#include "core.hpp"
#include "path.hpp"

static GameInstance* g_game_instance;
static IEngine* g_engine;

static void game_frame( IEngine* engine );
static void game_draw( IEngine* engine );

static void game_free()
{
    DELETE_OBJ(g_game_instance);
    g_game_instance = nullptr;

    g_engine->cleanup();
    DELETE_OBJ(g_engine);
    g_engine = nullptr;
}

void game_init(core::Path root)
{
    g_engine = NEW_OBJ(Engine, 0, nullptr);

    core::SystemManager& system_manager = g_engine->get_system_manager();

    system_manager.add_system<core::FileSystem>();
    system_manager.add_system<core::ResourceStorage>();
    system_manager.add_system<core::WidgetSystem>();

    if (g_engine->init(1024, 768, "RogueLike"))
    {
        glm::vec2 size = g_engine->get_window_size();

        g_game_instance = NEW_OBJ(GameInstance, g_engine, size.x, size.y);

        g_engine->set_callback(Draw, &game_draw);
        g_engine->set_callback(Frame, &game_frame);
        //g_engine->set_callback(Clean, &game_free);

        g_game_instance->init();
    }
}

static void game_frame( IEngine* engine )
{
    float delta = static_cast< float >( engine->get_frame_time( ) ) / 1000;

    g_game_instance->frame( delta );
}

static void game_draw( IEngine* engine )
{
    IRender* render = engine->get_render( );
    ASSERT( render != nullptr );

    g_game_instance->draw( render );
}

int game_loop()
{
    while (g_engine->is_runned())
    {
        g_engine->update();
    }
    
    game_free();
    
    return 0;
}