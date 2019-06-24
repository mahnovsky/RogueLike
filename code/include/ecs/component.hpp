#pragma once

#include "defines.hpp"

#define DECLARE_COMPONENT( CLASS_NAME )                    \
    static constexpr basic::uint32 TYPE_UID = __COUNTER__; \
    Entity* entity = nullptr;                              \
    basic::uint32 get_type_uid( ) const override           \
    {                                                      \
        return TYPE_UID;                                   \
    }                                                      \
    Entity* get_entity( ) override                         \
    {                                                      \
        return entity;                                     \
    }                                                      \
    ~CLASS_NAME( ) override                                \
    {                                                      \
    }

class IComponent
{
public:
    virtual ~IComponent( ) = default;

    virtual basic::uint32 get_type_uid( ) const = 0;

    virtual Entity* get_entity( ) = 0;
};

class IComponentStorage
{
public:
    virtual ~IComponentStorage( )
    {
    }

    virtual IComponent* create( ) = 0;

    virtual void release( IComponent* comp ) = 0;

    virtual const char* const get_name( ) const = 0;

    virtual basic::Vector< IComponent* > get_components( ) const = 0;
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

    ~ComponentStorage( ) override
    {
        for ( auto cmp : m_components )
        {
            m_pool.free( cmp );
        }
        m_components.clear( );
    }

    static basic::uint32
    get_component_id( )
    {
        return T::TYPE_UID;
    }

    IComponent*
    create( ) override
    {
        T* component = m_pool.make( );

        m_components.push( component );

        return component;
    }

    void
    release( IComponent* comp ) override
    {
        ASSERT( comp->get_type_uid( ) == T::TYPE_UID );
        T* c = dynamic_cast< T* >( comp );

        m_pool.free( c );
    }

    const char* const
    get_name( ) const override
    {
        return m_name;
    }

    basic::Vector< IComponent* >
    get_components( ) const override
    {
        basic::Vector< IComponent* > v;
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

private:
    const char* const m_name;
    basic::Pool< T > m_pool;
    basic::Vector< T* > m_components;
    // basic::HashMap< const Entity*, T* > m_links;
};
