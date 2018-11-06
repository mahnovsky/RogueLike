#include "engine.hpp"
#include "sprite.hpp"
#include "game/game_instance.hpp"

GameInstance* g_game_instance;

static void game_frame( Engine* engine );
static void game_draw( Engine* engine );

static Sprite g_back;

static void sprite_update( void* user_data )
{
    Sprite* sprite = static_cast<Sprite*>( user_data );

    glm::vec3 pos = sprite->get_position();
    pos.x += 0.03f;

    sprite->set_position( pos );
}

void game_init( Engine* engine )
{
    int w, h;
    engine->get_window_size( w, h );

    float width = static_cast<float>( w );
    float height = static_cast<float>( h );

    g_game_instance = new GameInstance( engine, width, height );

    engine->set_callback( Draw, &game_draw );
    engine->set_callback( Frame, &game_frame );

    g_game_instance->init();

    //TimerManager::get().add( { 0.5f, &sprite_update, &g_back, 8 } );
}

static void game_frame( Engine* engine )
{
    float delta = static_cast<float>(engine->get_frame_time()) / 1000;

    g_game_instance->frame(  delta );
}

static void game_draw( Engine* engine )
{
    IRender* render = engine->get_render();
    ASSERT( render != nullptr );

    g_game_instance->draw( render );
}



