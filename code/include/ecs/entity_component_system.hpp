#pragma once

#include "component.hpp"
#include "defines.hpp"

enum ComponentAction
{
    Attached,
    Updated,
    Detached
};

class ISystem
{
public:
    virtual ~ISystem( )
    {
    }

    virtual void on_component_event( Entity* ent, basic::uint32 component_id, ComponentAction act )
            = 0;
};

class EntityComponentSystem
{
public:
    EntityComponentSystem( );
    ~EntityComponentSystem( );

    void* create_component( const char* name );

    template < class T >
    T*
    create_component( )
    {
        auto id = ComponentStorage< T >::get_component_id( );
        ASSERT( id < m_storages.get_size( ) );

        void* vptr = m_storages[ id ]->create( );
        T* component = static_cast< T* >( vptr );

        return component;
    }

    template < class T >
    bool
    registry_component( const char* name )
    {
        auto id = get_component_id< T >( ) + 1;

        m_storages.push( NEW_OBJ( ComponentStorage< T >, name ) );

        ASSERT( id == m_storages.get_size( ) );

        return true;
    }

    Entity* create( );

    void destroy( Entity* ent );

    void update( float dt );

    template < class T >
    const basic::Vector< T* >&
    get_components( ) const
    {
        auto id = get_component_id< T >( );

        ComponentStorage< T >* storage = dynamic_cast< ComponentStorage< T >* >( m_storages[ id ] );

        return storage->get_components_t( );
    }

    template < class T >
    basic::uint32
    get_component_id( ) const
    {
        return ComponentStorage< T >::get_component_id( );
    }

    template < class T >
    void
    bind( Entity* ent, T* obj )
    {
        auto id = get_component_id< T >( );

        ComponentStorage< T >* storage = dynamic_cast< ComponentStorage< T >* >( m_storages[ id ] );

        storage->bind( ent, obj );
        emit( ent, id, ComponentAction::Attached );
    }

    template < class T >
    T*
    get_component( const Entity* ent )
    {
        auto id = get_component_id< T >( );

        ComponentStorage< T >* storage = dynamic_cast< ComponentStorage< T >* >( m_storages[ id ] );

        return storage->get( ent );
    }

    template < class T >
    T*
    create_system( )
    {
        T* sys = NEW_OBJ( T );

        m_systems.push( sys );

        return sys;
    }

    void subscribe( basic::uint32 component_id, ISystem* system );

    void unsubscribe( basic::uint32 component_id, ISystem* system );

    void emit( Entity* ent, basic::uint32 component_id, ComponentAction act );

private:
    basic::uint32 m_id_counter;
    basic::Vector< Entity* > m_entities;
    basic::Vector< Entity* > m_destroyed;
    basic::Vector< IComponentStorage* > m_storages;
    basic::Vector< ISystem* > m_systems;
    basic::HashMap< basic::uint32, basic::Vector< ISystem* > > m_subscribers;
};
