#pragma once

#include "basic/string.hpp"

class Object
{
public:
    Object();
    Object(const char* name);
    virtual ~Object();

    void set_tag( basic::int32 tag );

    basic::int32 get_tag() const;

    void set_name( const basic::String& name );

    const basic::String& get_name() const;

    void retain();

    void release();

    basic::ref_count get_refs() const;

private:
    basic::String m_name;
    basic::int32 m_tag;
};
