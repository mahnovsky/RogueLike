#pragma once

#include "vector.hpp"

namespace basic
{

Vector<char> get_file_content( const char* file_name );

bool is_dir_exist( const char* path );

}
