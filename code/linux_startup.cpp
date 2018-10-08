
#include "render.cpp"
#include "engine.cpp"
#include "render_object.cpp"
#include "platform/x11_window.cpp"
#include "basic/file.cpp"
#include "basic/util_functions.cpp"
#include "game.cpp"

void set_data_path(const char* bin)
{
    basic::String bin_file = bin;
    
    int pos = bin_file.find_last( '/' );

    if( pos >= 0 )
    {
        basic::String path = bin_file.get_substr( 0, pos );

        pos = path.find_last( '/' );
        if( pos >= 0 )
        {
            LOG("path %s", path.get_cstr() );

            basic::g_data_path = path.get_substr( 0, pos ) + "/data/";
        }
    }
}

int
main( int argc, char** argv )
{
    set_data_path( argv[0] );

    Engine engine( argc, argv );

    engine.set_callback( Init, &game_init );

    return engine.run( 1024, 768, "Game" );
}
