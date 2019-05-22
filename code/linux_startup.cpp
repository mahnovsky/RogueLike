#include "basic/file.hpp"
#include "engine.hpp"

#include <unistd.h>

using namespace basic;

namespace basic
{
extern String g_data_path;
}

#define FILENAME_MAX 1024

void game_init( Engine* );

void
set_data_path( const char* binary_path )
{
    basic::String path = binary_path;
    uint32 index = 0;

    if ( path.find_last( index, '/' ) && path.find_last( index, '/', index - 1 ) )
    {
        path = path.get_substr( 0, index ) + "/data/";
    }

    if ( is_dir_exist( path.get_cstr( ) ) )
    {
        basic::g_data_path = path;

        LOG( "path %s", path.get_cstr( ) );
    }
}

int
main( int argc, char** argv )
{
    set_data_path( argv[ 0 ] );

    IEngine* engine = NEW_OBJ( Engine, argc, argv );

    engine->set_callback( Init, &game_init );

    if ( engine->init( 1024, 768, "RogueLike" ) )
    {
        while ( engine->update( ) )
            ;

        engine->cleanup( );
    }

    DELETE_OBJ( engine );

    return EXIT_SUCCESS;
}
