#include "object_manager.hpp"

#include "object.hpp"

ObjectManager::ObjectManager( )
    : m_objects( )
    , m_typed_objects( )
{
    m_typed_objects.resize( static_cast< basic::uint32 >( SharedObjectType::End ) );
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

SharedObject*
ObjectManager::find( SharedObjectType type, const char* name )
{
    auto& cont = m_typed_objects[ static_cast< basic::uint32 >( type ) ];

    for ( auto obj : cont )
    {
        if ( obj->get_name( ) == name )
        {
            return obj;
        }
    }

    return nullptr;
}

basic::Vector< SharedObject* >
ObjectManager::get_all( SharedObjectType type ) const
{
    return m_typed_objects[ static_cast< basic::uint32 >( type ) ];
}

void
ObjectManager::add( SharedObject* obj )
{
    ASSERT( obj );

    m_objects.push( obj );

    auto type = obj->get_type( );
    m_typed_objects[ static_cast< basic::uint32 >( type ) ].push( obj );
}

void
ObjectManager::remove( SharedObject* obj )
{
    ASSERT( obj );

    auto type = obj->get_type( );
    auto& cont = m_typed_objects[ static_cast< basic::uint32 >( type ) ];

    basic::uint32 index;
    if ( cont.find_first( index, obj ) )
    {
        cont.swap_remove( index );
    }

    if ( m_objects.find_first( index, obj ) )
    {
        m_objects.swap_remove( index );
    }
}
