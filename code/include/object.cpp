#include "object.h"

#include "basic/util_functions.hpp"

Object::Object()
    :m_name()
{
}

Object::~Object()
{
}

void Object::set_name(const basic::String &name)
{
    m_name = name;
}

const basic::String &Object::get_name() const
{
    return m_name;
}

void Object::retain()
{
    basic::increment_ref( this );
}

void Object::release()
{
    if( basic::decrement_ref( this ) <= 0 )
    {
        delete this;
    }
}
