#include "basic/file.hpp"
#include "basic/string.hpp"

#include <cstdio>

namespace basic
{

String g_data_path = "./data/";

Vector<uint8> get_file_content( const char* file_name )
{
    Vector<uint8> result;
    String file_path = g_data_path + file_name;
    FILE* file = fopen( file_path.get_cstr(), "rb" );

    if( file )
    {
        fseek( file, 0, SEEK_END );
        long size = ftell( file );
        fseek( file, 0, SEEK_SET );

        result.resize( static_cast<uint32>( size ) );
        fread( result.get_raw(), sizeof(char), static_cast<size_t>( size ), file );

        fclose( file );
    }

    return result;
}

}
