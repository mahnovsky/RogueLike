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

    basic::Vector<basic::String> get_values( const char* key ) const;

private:
    basic::Vector<Pair> m_values;
};
