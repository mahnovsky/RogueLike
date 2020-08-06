#pragma once

#include "vector.hpp"

#include <vector>
#include <string>

namespace basic
{

std::vector<uint8> get_file_content( const char* file_name );

bool is_dir_exist( const char* path );

extern std::string g_data_path;

}
