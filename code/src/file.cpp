#include "basic/file.hpp"

#include <cstdio>

namespace basic
{

Vector<char> get_file_content( const char* file_name )
{
    Vector<char> result;
    FILE* file = fopen( file_name, "r" );

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
