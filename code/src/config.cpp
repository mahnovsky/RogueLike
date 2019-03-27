#include "config.hpp"

Config::Config( ObjectManager* manager, const char* file )
    :FileResource (manager, file)
    ,m_values()
{
}

Config::~Config()
{
}

bool Config::load(ResourceStorage *)
{
    basic::Vector<basic::uint8> data = basic::get_file_content( get_name().get_cstr() );

    if( !data.is_empty() && m_doc.load( reinterpret_cast<char*>(data.get_raw()) ) )
    {
        return true;
    }

    return false;
}

Config *Config::create(ObjectManager* manager, const char *file)
{
    return NEW_OBJ(Config, manager, file);
}

const basic::JsonObject* Config::get_values(const char *key) const
{
    const basic::JsonObject* root = m_doc.get_root();

    return root->get_object(key);
}
