#include "basic/file.hpp"
#include "basic/string.hpp"

#include <cstdio>

namespace basic
{

String g_data_path = "./data/";

Vector<char> get_file_content( const char* file_name )
{
    Vector<char> result;
    String file_path = g_data_path + file_name;
    FILE* file = fopen( file_path.get_cstr(), "r" );

    if( file )
    {
        fseek( file, 0, SEEK_END );
        size_t size = ftell( file );
        fseek( file, 0, SEEK_SET );

        result.resize( size );
        fread( result.get_raw(), sizeof(char), size, file ); 

        fclose( file );
    }

    return result;
}

}
