#include "basic/file.hpp"
#include "basic/string.hpp"

#include <cstdio>
#include <vector>
#include <string>

namespace basic
{
std::string g_data_path = "./data/";

std::vector< uint8_t > get_file_content( const char* file_name )
{
    std::vector< uint8_t > result;
    std::string file_path = g_data_path + file_name;
    FILE* file = fopen( file_path.c_str( ), "rb" );

    if ( file )
    {
        fseek( file, 0, SEEK_END );
        long size = ftell( file );
        fseek( file, 0, SEEK_SET );

        result.resize( static_cast< uint32 >( size ) );
        fread( result.data( ), sizeof( char ), static_cast< size_t >( size ), file );

        fclose( file );
    }

    return result;
}
}
