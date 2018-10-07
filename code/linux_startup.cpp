#include "basic/vector.hpp"
#include "basic/string.hpp"
#include "engine.hpp"
#include "render_object.hpp"

#include "engine.cpp"
#include "render_object.cpp"
#include "render.cpp"
#include "glew.c"
#include "basic/file.cpp"
#include "basic/util_functions.cpp"
#include "platform/x11_window.cpp"
#include "game.cpp"

int
main( int argv, char** argc )
{
    Engine engine( argv, argc );

    engine.set_callback( Init, &game_init );

    return engine.run( 1024, 768, "Game" );
}