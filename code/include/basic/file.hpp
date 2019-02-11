#pragma once

#include "vector.hpp"

namespace basic
{

class String;

Vector<uint8> get_file_content( const char* file_name );

bool is_dir_exist( const char* path );

extern String g_data_path;

}
