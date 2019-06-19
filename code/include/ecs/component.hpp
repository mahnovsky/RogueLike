#pragma once

#include "defines.hpp"

class IComponentStorage
{
public:
    virtual ~IComponentStorage( )
    {
    }

    virtual void* create( ) = 0;

    virtual void release( void* comp ) = 0;

    virtual const char* const get_name( ) const = 0;

    virtual basic::Vector< void* > get_components( ) const = 0;
};

template < class T >
class ComponentStorage : public IComponentStorage
{
public:
    ComponentStorage( const char* const name )
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
        // static basic::uint32 id = IComponentStorage::generate_type_id( );

        return T::TYPE_UID;
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

    const char* const
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

    const basic::Vector< T* >&
    get_components_t( ) const
    {
        return m_components;
    }

    void
    bind( const Entity* ent, T* component )
    {
        m_links.insert( ent, component );
    }

    T*
    get( const Entity* ent )
    {
        T* obj = nullptr;

        m_links.find( ent, obj );

        return obj;
    }

private:
    const char* const m_name;
    basic::Pool< T > m_pool;
    basic::Vector< T* > m_components;
    basic::HashMap< const Entity*, T* > m_links;
};
