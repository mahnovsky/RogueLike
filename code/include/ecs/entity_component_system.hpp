#pragma once

#include "defines.hpp"
#include "entity.hpp"

class IComponent;

class IComponentStorage
{
public:
    virtual ~IComponentStorage( )
    {
    }

    virtual IComponent* create( ) = 0;

    virtual basic::String get_name( ) const = 0;

    virtual basic::Vector< IComponent* > get_components( ) const = 0;
};

template < class T >
class ComponentStorage : public IComponentStorage
{
public:
    ComponentStorage( const char* name )
        : m_name( name )
        , m_components( )
    {
    }

    IComponent*
    create( ) override
    {
        m_components.emplace( T{m_name.get_cstr( )} );
        IComponent* component = &m_components.back( );
        m_pcomponents.push( component );
        return component;
    }

    basic::String
    get_name( ) const override
    {
        return m_name;
    }

    basic::Vector< IComponent* >
    get_components( ) const override
    {
        return m_pcomponents;
    }

private:
    basic::String m_name;
    basic::Vector< T > m_components;
    basic::Vector< IComponent* > m_pcomponents;
};

class ISystem
{
public:
    virtual ~ISystem( )
    {
    }

    virtual void update( EntityComponentSystem* ecs ) = 0;
};

class EntityComponentSystem
{
public:
    EntityComponentSystem( );
    ~EntityComponentSystem( );

    IComponent* create_component( const char* name );

    template < class T >
    T*
    create_component( )
    {
        ASSERT( T::_type_id < m_storages.get_size( ) );

        void* vptr = m_storages[ T::_type_id ]->create( );
        T* component = static_cast< T* >( vptr );

        return component;
    }

    template < class T >
    bool
    registry_component( const char* name )
    {
        auto id = T::_type_id;

        if ( m_storages[ id ] != nullptr )
        {
            LOG( "failed register, component already exist" );
            return false;
        }

        m_storages[ id ] = NEW_OBJ( ComponentStorage< T >, name );
        return true;
    }

    Entity* create( );

    void destroy( Entity* ent );

    void update( float dt );

private:
    EntityID m_id_counter;
    basic::Vector< Entity* > m_entities;
    basic::Vector< Entity* > m_destroyed;
    basic::Vector< IComponentStorage* > m_storages;
    basic::Vector< ISystem* > m_systems;
};
