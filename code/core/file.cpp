#include "file.hpp"
#include "string.hpp"
#include "path.hpp"

#include <cstdio>
#include <vector>
#include <string>

namespace core
{
std::string g_data_path = "./data/";

std::vector< uint8_t > get_file_content( const char* file_name )
{
    std::vector< uint8_t > result;
    std::string file_path = g_data_path + file_name;
 /*
    FILE* file = fopen( file_path.c_str( ), "rb" );

    if ( file )
    {
        fseek( file, 0, SEEK_END );
        long size = ftell( file );
        fseek( file, 0, SEEK_SET );

        result.resize( static_cast< uint32_t >( size ) );
        fread( result.data( ), sizeof( char ), static_cast< size_t >( size ), file );

        fclose( file );
    }
    */
    std::unique_ptr<IFileNode> node = open_file(Path::parse(file_path, "/"));
    if (node && node->get_type() == FileNodeType::File)
    {
        node->get_content(result);
    }

    return result;
}
}
