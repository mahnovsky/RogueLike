#include "object.hpp"

#include "basic/memory.hpp"
#include "object_manager.hpp"

SharedObject::SharedObject( ObjectManager* manager )
    : m_manager( manager )
    , m_refs( 0 )
    , m_tag( 0 )
    , m_name( )
{
    manager->add( this );
}

SharedObject::SharedObject( ObjectManager* manager, const char* name )
    : m_manager( manager )
    , m_refs( 0 )
    , m_tag( 0 )
    , m_name( name )
{
    manager->add( this );
}

SharedObject::~SharedObject( )
{
    m_manager->remove( this );
}

void
SharedObject::set_tag( basic::int32 tag )
{
    m_tag = tag;
}

basic::int32
SharedObject::get_tag( ) const
{
    return m_tag;
}

void
SharedObject::set_name( const basic::String& name )
{
    m_name = name;
}

const basic::String&
SharedObject::get_name( ) const
{
    return m_name;
}

void
SharedObject::retain( )
{
    ++m_refs;
}

void
SharedObject::release( )
{
    --m_refs;
    if ( m_refs <= 0 )
    {
        DELETE_OBJ( this );
    }
}

basic::int32
SharedObject::get_refs( ) const
{
    return m_refs;
}

ObjectManager*
SharedObject::get_manager( )
{
    return m_manager;
}
