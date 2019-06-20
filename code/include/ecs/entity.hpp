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

        comp->entity = this;

        m_components.insert( T::TYPE_UID, comp );

        m_ecs->emit( this, T::TYPE_UID, ComponentAction::Attached );

        return comp;
    }

    template < class T >
    T*
    get_component( )
    {
        T* comp = nullptr;
        if ( size( m_components ) > T::TYPE_UID && m_components[ T::TYPE_UID ] != nullptr )
        {
            comp = dynamic_cast< T* >( m_components[ T::TYPE_UID ] );
        }
        return comp;
    }

    template < class T >
    const T*
    get_component( ) const
    {
        T* comp = nullptr;
        if ( size( m_components ) > T::TYPE_UID )
        {
            comp = m_components[ T::TYPE_UID ];
        }
        return comp;
    }

    EntityID get_uid( ) const;

    virtual void on_destroy( );

private:
    const EntityID m_uid;
    EntityComponentSystem* m_ecs;
    basic::Vector< IComponent* > m_components;
};
