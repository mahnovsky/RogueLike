#pragma once

#include "component.hpp"
#include "entity_component_system.hpp"

using EntityID = basic::uint32;

class Entity
{
public:
    Entity( EntityID id, EntityComponentSystem* ecs );
    virtual ~Entity( );

    template < class T >
    T*
    add_component( )
    {
        T* comp = m_ecs->create_component< T >( );

        m_ecs->bind( m_uid, comp );

        return comp;
    }

    template < class T >
    T*
    get_component( )
    {
        return m_ecs->get_component< T >( m_uid );
    }

    template < class T >
    const T*
    get_component( ) const
    {
        // const auto ecs = m_ecs;
        return m_ecs->get_component< T >( m_uid );
    }

    EntityID get_id( ) const;

    virtual void on_destroy( );

private:
    const EntityID m_uid;
    EntityComponentSystem* m_ecs;
};
