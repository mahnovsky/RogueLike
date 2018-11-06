#include "basic/file.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
namespace basic
{
bool
is_dir_exist( const char* path )
{
    bool is_exist = false;

    struct stat st;
    if ( stat( path, &st ) == 0 && ( st.st_mode & S_IFDIR ) != 0 )
    {
        is_exist = true;
    }

    return is_exist;
}
}
