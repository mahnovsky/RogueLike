
#include "render.cpp"
#include "engine.cpp"
#include "render_object.cpp"
#include "transform.cpp"
#include "timer_manager.cpp"
#include "texture.cpp"
#include "sprite.cpp"

#include "platform/x11_window.cpp"
#include "platform/unix_time.cpp"
#include "platform/unix_file.cpp"

#include "basic/file.cpp"
#include "basic/util_functions.cpp"
#include "basic/image.cpp"

#include "game.cpp"


#include <unistd.h>

void set_data_path()
{
    char current_path[FILENAME_MAX];

    if ( !getcwd( current_path, sizeof(current_path) ) )
    {
        return ;
    }

    basic::String path = current_path;
    path += "/data/";

    if( is_dir_exist( path.get_cstr() ) )
    {
        basic::g_data_path = path;        

        LOG("path %s", path.get_cstr() );

        return;
    }    

    basic::String bin_file = current_path;
    
    int pos = bin_file.find_last( '/' );

    LOG( "init data path %s, %d", bin_file.get_cstr(), pos );

    if( pos >= 0 )
    {
        path = std::move( bin_file.get_substr( 0, pos + 1 ) );

        LOG("path %s", path.get_cstr() );

        basic::g_data_path = path + "data/";

        ASSERT( is_dir_exist( basic::g_data_path.get_cstr() ) );
    }
}

int
main( int argc, char** argv )
{
    set_data_path( );

    Engine engine( argc, argv );

    engine.set_callback( Init, &game_init );

    return engine.run( 1024, 768, "Game" );
}
