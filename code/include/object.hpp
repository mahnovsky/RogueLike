#pragma once

#include "defines.hpp"

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

    basic::int32 get_refs() const;

private:
	basic::int32 m_refs;
	basic::int32 m_tag;
    basic::String m_name;
};
