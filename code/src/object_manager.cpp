#include "object_manager.hpp"

#include "object.hpp"

ObjectManager::ObjectManager()
{
}

ObjectManager::~ObjectManager()
{
}

Object *ObjectManager::find(const char *name)
{
    for(basic::uint32 i = 0; i < m_objects.get_size(); ++i)
    {
        if( m_objects[i]->get_name() == name )
        {
            return m_objects[i];
        }
    }

    return nullptr;
}

Object *ObjectManager::find(basic::int32 tag)
{
    for(basic::uint32 i = 0; i < m_objects.get_size(); ++i)
    {
        if( m_objects[i]->get_tag() == tag )
        {
            return m_objects[i];
        }
    }

    return nullptr;
}

Object *ObjectManager::find(const char *name, basic::int32 tag)
{
    for(basic::uint32 i = 0; i < m_objects.get_size(); ++i)
    {
        if( m_objects[i]->get_tag() == tag &&
            m_objects[i]->get_name() == name )
        {
            return m_objects[i];
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
