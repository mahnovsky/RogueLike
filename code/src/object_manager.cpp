#include "object_manager.hpp"

#include "object.hpp"

ObjectManager::ObjectManager( )
    : m_objects( )
{
}

ObjectManager::~ObjectManager( )
{
}

SharedObject*
ObjectManager::find( const char* name )
{
    for ( SharedObject* object : m_objects )
    {
        if ( object->get_name( ) == name )
        {
            return object;
        }
    }

    return nullptr;
}

SharedObject*
ObjectManager::find( basic::int32 tag )
{
    for ( SharedObject* object : m_objects )
    {
        if ( object->get_tag( ) == tag )
        {
            return object;
        }
    }

    return nullptr;
}

SharedObject*
ObjectManager::find( const char* name, basic::int32 tag )
{
    for ( SharedObject* object : m_objects )
    {
        if ( object->get_tag( ) == tag && object->get_name( ) == name )
        {
            return object;
        }
    }

    return nullptr;
}

void
ObjectManager::add( SharedObject* obj )
{
    m_objects.push( obj );
}

void
ObjectManager::remove( SharedObject* obj )
{
    basic::uint32 index = 0;
    if ( m_objects.find_first( index, obj ) )
    {
        m_objects.swap_remove( index );
    }
}
