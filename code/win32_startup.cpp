#include <windows.h>

#define __PRETTY_FUNCTION__ __FUNCTION__

#include "engine.cpp"
#include "render_object.cpp"
#include "render.cpp"
#include "glew.c"
#include "basic/file.cpp"
#include "basic/util_functions.cpp"
#include "platform/win32_window.cpp"
#include "game.cpp"

HINSTANCE g_instance;

int __stdcall WinMain( HINSTANCE instance,
                       HINSTANCE prev_instance,
                       LPSTR cmd,
                       int show )
{
    g_instance = instance;

    Engine engine( 0, 0 );

    engine.set_callback( Init, &game_init );

    return engine.run( 1024, 768, "Game" );
}
