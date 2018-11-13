#include "render_common.hpp"
#include "basic/file.hpp"

bool load_mesh(const char *file, Mesh &out_mesh)
{
    basic::Vector<char> data{ std::move( basic::get_file_content( file ) ) };

    if( !data.is_empty() )
    {

    }

    return true;
}
