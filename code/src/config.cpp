#include "config.hpp"

#include "basic/memory.hpp"
#include "basic/hash_map.hpp"
#include "cJSON/cJSON.h"

Config::Config( ObjectManager* manager, const char* file )
    :FileResource (manager, file)
    ,m_values()
{
}

Config::~Config()
{
}

static void* alloc(size_t sz)
{
    return basic::mem_alloc(static_cast<basic::uint32>(sz));
}

static void free_mem(void* ptr)
{
    basic::mem_free(ptr);
}

bool Config::load(ResourceStorage *)
{
    static cJSON_Hooks hoocks;
    if( !hoocks.malloc_fn )
    {
        hoocks.malloc_fn = alloc;
        hoocks.free_fn = free_mem;
        cJSON_InitHooks(&hoocks);
    }

    basic::Vector<basic::uint8> data = basic::get_file_content( get_name().get_cstr() );

    cJSON* json = nullptr;
    if(!data.is_empty())
    {
        json = cJSON_Parse(reinterpret_cast<char*>(data.get_raw()));
        basic::HashMap<int, char> m;

        int c = 0;
        for(int i = 0; i < 50; ++i)
        {
            int pos = i + 'A';
            m.insert(pos, (char)pos);
            c += i;
        }

        int counter = 0;
        for(auto& p : m)
        {
            LOG("(%d, %c)", p.key, p.value);
            ++counter;
        }

        basic::HashMap<int, char>::Iterator found = m.find(5);
        if( found != m.end() )
        {
            LOG("%c", found->value);
        }

        cJSON_Delete(json);
    }

    if( !data.is_empty() )
    {
        Pair p;
        basic::uint32 offset = 0;
        do
        {
			basic::String line;
			offset += basic::String::
                    read_line( reinterpret_cast<basic::char_t*>(data.get_raw() + offset),
                               data.get_size() - offset, line );

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

Config *Config::create(ObjectManager* manager, const char *file)
{
    return NEW_OBJ(Config, manager, file);
}

basic::Vector<basic::String> Config::get_values(const char *key) const
{
    for(const auto& item : m_values)
    {
        if(item.key == key)
        {
            return  item.values;
        }
    }

    return basic::Vector<basic::String>();
}
