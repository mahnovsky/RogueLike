#include "object.hpp"

#include "basic/memory.hpp"
#include "object_manager.hpp"

Object::Object(ObjectManager *manager)
    : m_manager(manager)
    , m_refs(0)
    , m_tag(0)
    , m_name()
{
    manager->add( this );
}

Object::Object(ObjectManager *manager, const char *name)
    : m_manager(manager)
    , m_refs(0)
    , m_tag(0)
	, m_name(name)
{
    manager->add( this );
}

Object::~Object()
{
    m_manager->remove( this );
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
        DELETE_OBJ(this);
    }
}

basic::int32 Object::get_refs() const
{
    return m_refs;
}

ObjectManager *Object::get_manager()
{
    return m_manager;
}
