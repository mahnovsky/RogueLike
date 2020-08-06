#include "config.hpp"

Config::Config( const char* file )
    : FileResource( file )
    , m_values( )
{
}

Config::~Config( )
{
}

bool Config::load( ResourceStorage* )
{
   auto data = basic::get_file_content( get_file_name( ).data() );

    if ( !data.empty( ) && m_doc.load( reinterpret_cast< char* >( data.data( ) ) ) )
    {
        return true;
    }

    return false;
}

Config* Config::create( const char* file )
{
    return NEW_OBJ( Config, file );
}

const basic::JsonObject*
Config::get_values( const char* key ) const
{
    const basic::JsonObject* root = m_doc.get_root( );

    return root->get_object( key );
}
