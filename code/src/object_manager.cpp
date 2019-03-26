#include "object_manager.hpp"

#include "object.hpp"

ObjectManager::ObjectManager()
    :m_objects()
{
}

ObjectManager::~ObjectManager()
{
}

Object *ObjectManager::find(const char *name)
{
    for(Object* object : m_objects)
    {
        if(object->get_name() == name)
        {
            return object;
        }
    }

    return nullptr;
}

Object *ObjectManager::find(basic::int32 tag)
{
    for(Object* object : m_objects)
    {
        if(object->get_tag() == tag)
        {
            return object;
        }
    }

    return nullptr;
}

Object *ObjectManager::find(const char *name, basic::int32 tag)
{
    for(Object* object : m_objects)
    {
        if( object->get_tag() == tag &&
            object->get_name() == name )
        {
            return object;
        }
    }

    return nullptr;
}

void ObjectManager::add( Object *obj )
{
    m_objects.push( obj );
}

void ObjectManager::remove( Object *obj )
{
    basic::uint32 index = 0;
    if( m_objects.find_first(index, obj))
    {
        m_objects.swap_remove( index );
    }
}
