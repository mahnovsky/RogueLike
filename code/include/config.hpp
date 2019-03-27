#pragma once

#include "resource_storage.hpp"

class Config : public FileResource
{
    struct Pair
    {
        basic::String key;
        basic::Vector<basic::String> values;
    };

public:
    Config( ObjectManager* manager, const char* file );
    ~Config() override;

    bool load( ResourceStorage* ) override;

    static Config* create(ObjectManager *manager, const char* file );

   const basic::JsonObject* get_values( const char* key ) const;

    basic::int32 get_int( const char* key ) const;

    float get_float( const char* key ) const;

    const char* get_string( const char* key ) const;

private:
    basic::Vector<Pair> m_values;
    basic::JsonDocument m_doc;
};
