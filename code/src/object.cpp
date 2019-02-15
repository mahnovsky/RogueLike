#include "object.hpp"

#include "basic/memory.hpp"

Object::Object()
    : m_name()
    , m_tag(0)
{
}

Object::Object(const char *name)
    : m_name(name)
    , m_tag(0)
{
}

Object::~Object()
{
}

void Object::set_tag(basic::int32 tag)
{
    m_tag = tag;
}

basic::int32 Object::get_tag() const
{
    return m_tag;
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

basic::ref_count Object::get_refs() const
{
    return basic::get_refs( const_cast<Object*>(this) );
}
