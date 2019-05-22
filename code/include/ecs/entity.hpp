#pragma once

#include "component.hpp"

using EntityID = basic::uint32;

class Entity
{
public:
    Entity( EntityID id );
    virtual ~Entity( );

    virtual void initialize( );

    void add_component( IComponent* component );

    template < class T >
    T*
    get_component( )
    {
        IComponent* comp = find_component( T::_type_id );
        if ( comp )
        {
            void* vptr = static_cast< void* >( comp );
            return static_cast< T* >( comp );
        }
        return nullptr;
    }

    IComponent* find_component( ComponentTypeID id );

    EntityID get_id( ) const;

    virtual void on_destroy( );

protected:
    void component_event_broadcast( IComponent* comp, bool attached );

private:
    const EntityID m_entity_id;
    basic::Vector< IComponent* > m_components;
};
