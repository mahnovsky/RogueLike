#pragma once

#include "defines.hpp"

class Entity;

class IComponentStorage
{
public:
    virtual ~IComponentStorage( )
    {
    }

    virtual void* create( ) = 0;

    virtual void release( void* comp ) = 0;

    virtual basic::String get_name( ) const = 0;

    virtual basic::Vector< void* > get_components( ) const = 0;

    static basic::uint32
    generate_type_id( )
    {
        static basic::uint32 counter;

        return counter++;
    }
};

template < class T >
class ComponentStorage : public IComponentStorage
{
public:
    ComponentStorage( const char* name )
        : m_name( name )
        , m_pool( 100 )
        , m_components( )
    {
    }

    ~ComponentStorage( )
    {
    }

    static basic::uint32
    get_component_id( )
    {
        static basic::uint32 id = IComponentStorage::generate_type_id( );

        return id;
    }

    void*
    create( ) override
    {
        T* component = m_pool.make( );

        m_components.push( component );

        return static_cast< void* >( component );
    }

    void
    release( void* comp ) override
    {
        T* c = static_cast< T* >( comp );

        m_pool.free( c );
    }

    basic::String
    get_name( ) const override
    {
        return m_name;
    }

    basic::Vector< void* >
    get_components( ) const override
    {
        basic::Vector< void* > v;
        v.reserve( m_components.get_size( ) );
        for ( auto c : m_components )
        {
            v.push( c );
        }

        return v;
    }

    basic::Vector< T* >
    get_components_t( ) const
    {
        return m_components;
    }

    void
    bind( basic::uint32 key, T* component )
    {
        m_links.insert( key, component );
    }

    T*
    get( basic::uint32 key )
    {
        T* obj;
        if ( m_links.find( key, obj ) )
        {
            return obj;
        }
        return nullptr;
    }

private:
    basic::String m_name;
    basic::Pool< T > m_pool;
    basic::Vector< T* > m_components;
    basic::HashMap< basic::uint32, T* > m_links;
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
    basic::Vector< T* >
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
    bind( basic::uint32 k, T* obj )
    {
        auto id = get_component_id< T >( );

        ComponentStorage< T >* storage = dynamic_cast< ComponentStorage< T >* >( m_storages[ id ] );

        storage->bind( k, obj );
    }

    template < class T >
    T*
    get_component( basic::uint32 k )
    {
        auto id = get_component_id< T >( );

        ComponentStorage< T >* storage = dynamic_cast< ComponentStorage< T >* >( m_storages[ id ] );

        return storage->get( k );
    }

private:
    basic::uint32 m_id_counter;
    basic::Vector< Entity* > m_entities;
    basic::Vector< Entity* > m_destroyed;
    basic::Vector< IComponentStorage* > m_storages;
    basic::Vector< ISystem* > m_systems;
};
