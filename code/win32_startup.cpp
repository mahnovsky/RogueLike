#include <windows.h>

#define __PRETTY_FUNCTION__ __FUNCTION__

#include "engine.cpp"
#include "render_object.cpp"
#include "render.cpp"
#include "file.cpp"
#include "glew.c"
#include "basic/util_functions.cpp"
#include "platform/win32_window.cpp"

int __stdcall WinMain( HINSTANCE instance,
                        HINSTANCE prev_instance,
                        LPSTR cmd,
                        int show )
{
    Engine engine( 0, 0 );

    return engine.run( 1024, 768, "Game" );
}
