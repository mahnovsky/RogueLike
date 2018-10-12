#include "engine.hpp"
#include "sprite.hpp"
#include "game/game_instance.cpp"

GameInstance g_game_instance;

static void game_frame( Engine* engine );
static void game_draw( Engine* engine );

static Sprite g_back;

static void game_init( Engine* engine )
{
    engine->set_callback( Draw, &game_draw );
    engine->set_callback( Frame, &game_frame );

    g_back.set_color( 255, 0, 0, 255 );
    g_back.init();

    g_game_instance.init();
}

static void game_frame( Engine* engine )
{
    static bool first = true;
    if( first )
    {
        g_back.set_color( 0, 255, 0, 255 ); 
        first = false;
    }

    glm::vec3 pos = g_back.get_position();
    pos.x += 0.01f;

    g_back.set_position( pos );

    g_game_instance.frame();
}

static void game_draw( Engine* engine )
{
    IRender* render = engine->get_render();
    ASSERT( render != nullptr );

    g_back.draw( render );

    g_game_instance.draw( render );
}



