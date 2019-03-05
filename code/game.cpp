#include "engine.hpp"
#include "game/game_instance.hpp"
#include "sprite.hpp"

static GameInstance* g_game_instance;

static void game_frame( Engine* engine );
static void game_draw( Engine* engine );

static void
game_free( Engine* )
{
    DELETE_OBJ(g_game_instance);
    g_game_instance = nullptr;
}

void
game_init( Engine* engine )
{
    glm::vec2 size = engine->get_window_size( );

    g_game_instance = NEW_OBJ(GameInstance, engine, size.x, size.y);

    engine->set_callback( Draw, &game_draw );
    engine->set_callback( Frame, &game_frame );
    engine->set_callback( Clean, &game_free );

    g_game_instance->init( );
}

static void
game_frame( Engine* engine )
{
    float delta = static_cast< float >( engine->get_frame_time( ) ) / 1000;

    g_game_instance->frame( delta );
}

static void
game_draw( Engine* engine )
{
    IRender* render = engine->get_render( );
    ASSERT( render != nullptr );

    g_game_instance->draw( render );
}
