#pragma once

#include "basic/string.hpp"

class Object
{
public:
    Object();
    virtual ~Object();

    void set_name( const basic::String& name );

    const basic::String& get_name() const;

    void retain();

    void release();

private:
    basic::String m_name;
};
