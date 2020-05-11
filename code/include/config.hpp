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
    GENERIC_OBJECT_IMPL(Config, NS_RESOURCE_TYPE);

    Config( GenericObjectManager* manager, const char* file );
    ~Config() override;

    bool load( ResourceStorage* ) ;

    static Config* create(GenericObjectManager *manager, const char* file );

    const basic::JsonObject* get_values( const char* key ) const;

private:
    basic::Vector<Pair> m_values;
    basic::JsonDocument m_doc;
};
