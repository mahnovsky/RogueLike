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

        m_ecs->bind( this, comp );
        m_components.push( m_ecs->get_component_id< T >( ) );

        return comp;
    }

    template < class T >
    T*
    get_component( )
    {
        return m_ecs->get_component< T >( this );
    }

    template < class T >
    const T*
    get_component( ) const
    {
        return m_ecs->get_component< T >( this );
    }

    EntityID get_uid( ) const;

    virtual void on_destroy( );

private:
    const EntityID m_uid;
    EntityComponentSystem* m_ecs;
    basic::Vector< basic::uint32 > m_components;
};
