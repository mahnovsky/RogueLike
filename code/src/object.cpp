#include "object.hpp"

#include "basic/memory.hpp"

Object::Object()
    : m_name()
    , m_tag(0)
{
}

Object::Object(const char *name)
    : m_refs(0)
	, m_name(name)
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
	++m_refs;
}

void Object::release()
{
	--m_refs;
    if( m_refs <= 0 )
    {
        delete this;
    }
}

basic::int32 Object::get_refs() const
{
    return m_refs;
}
