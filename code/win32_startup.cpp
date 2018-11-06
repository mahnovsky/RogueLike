
#include <windows.h>
#include "engine.hpp"

void game_init(Engine*);

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
