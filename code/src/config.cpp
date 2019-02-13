#include "config.hpp"


Config::Config( const char* file )
    :FileResource (file)
{
}

Config::~Config()
{
}

bool Config::load(ResourceStorage *)
{
    basic::Vector<basic::uint8> data = basic::get_file_content( get_name().get_cstr() );

    if( !data.is_empty() )
    {
        Pair p;
        basic::uint32 offset = 0;
        do
        {
            basic::String line = basic::String::
                    read_line( reinterpret_cast<basic::char_t*>(data.get_raw() + offset),
                               data.get_size() - offset );

            if( !line.is_empty() )
            {
                basic::uint32 from;
                basic::uint32 to;

                if(  line.find_first( from, '[' ) && line.find_last( to, ']') )
                {
                    if( !p.key.is_empty() && !p.values.is_empty() )
                    {
                        m_values.push( p );
                        p.values.clear();
                    }

                    ++from;
                    --to;
                    if( to > from )
                    {
                        p.key = line.get_substr( from, to );
                    }
                }
                else
                {
                    p.values.push( line );
                }
            }

            offset += line.get_size();
        }
        while( offset < data.get_size() );

        if( !p.key.is_empty() && !p.values.is_empty() )
        {
            m_values.push( p );
        }

        return !m_values.is_empty();
    }

    return false;
}

Config *Config::create(const char *file)
{
    return new Config( file );
}

basic::Vector<basic::String> Config::get_values(const char *key) const
{
    for(basic::uint32 i = 0; i < m_values.get_size(); ++i)
    {
        if( m_values[i].key == key )
        {
            return m_values[i].values;
        }
    }

    return basic::Vector<basic::String>();
}
