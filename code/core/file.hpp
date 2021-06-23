#pragma once

#include "vector.hpp"
#include "file_node.hpp"
#include "path.hpp"

#include <vector>
#include <string>
#include <memory>

namespace core
{

std::vector<uint8_t> get_file_content( const char* file_name );

bool is_dir_exist( const char* raw_path);

bool is_path_valid(const char* raw_path);

std::unique_ptr<IFileNode> open_file(const Path& path);

Path get_binary_path();

extern std::string g_data_path;

}
