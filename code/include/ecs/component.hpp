#pragma once

#include "defines.hpp"

using ComponentTypeID = basic::uint32;

extern ComponentTypeID components_count;

template < class T, class U >
T*
fast_cast( U* ptr )
{
    return static_cast< T* >( static_cast< void* >( ptr ) );
}

template < class T, class U >
const T*
fast_cast( const U* ptr )
{
    return static_cast< const T* >( static_cast< const void* >( ptr ) );
}

class IComponent
{
public:
    static ComponentTypeID
    generate_type_id( )
    {
        static ComponentTypeID counter;
        components_count = ++counter;

        return counter - 1;
    }

    template < class T >
    static ComponentTypeID
    type_id( )
    {
        static const ComponentTypeID id = IComponent::generate_type_id( );

        return id;
    }

    virtual ~IComponent( )
    {
    }

    virtual ComponentTypeID get_type_id( ) const = 0;

    virtual Entity* get_entity( ) = 0;

    virtual const Entity* get_entity( ) const = 0;

    virtual void on_attached( Entity* entity ) = 0;

    virtual void on_detached( Entity* entity ) = 0;

    virtual void on_attached_component( IComponent* comp ) = 0;

    virtual void on_detached_component( IComponent* comp ) = 0;

    virtual void add_listener( IComponent* comp ) = 0;

    virtual void remove_listener( IComponent* comp ) = 0;
};

template < class T >
class Component : public IComponent
{
public:
    static const ComponentTypeID _type_id;

    Component( const char* name )
        : m_entity( nullptr )
        , m_name( name )
    {
    }

    ComponentTypeID
    get_type_id( ) const override
    {
        return _type_id;
    }

    Entity*
    get_entity( ) override
    {
        return m_entity;
    }

    const Entity*
    get_entity( ) const override
    {
        return m_entity;
    }

    void
    on_attached( Entity* entity ) override
    {
        m_entity = entity;
    }

    void
    on_detached( Entity* ) override
    {
        m_entity = nullptr;
    }

    void
    on_attached_component( IComponent* ) override
    {
    }

    void
    on_detached_component( IComponent* ) override
    {
    }

    virtual void
    add_listener( IComponent* comp ) override
    {
        basic::uint32 index;
        if ( !m_listeners.find_first( index, comp ) )
        {
            m_listeners.push( comp );
        }
    }

    virtual void
    remove_listener( IComponent* comp ) override
    {
        basic::uint32 index;
        if ( m_listeners.find_first( index, comp ) )
        {
            m_listeners.swap_remove( index );
        }
    }

private:
    Entity* m_entity;
    basic::String m_name;
    basic::Vector< IComponent* > m_listeners;
};

template < class T >
const ComponentTypeID Component< T >::_type_id = IComponent::type_id< T >( );

class GroupTransformComponent;

class TransformComponent : public Component< TransformComponent >
{
public:
    TransformComponent( const char* name );

    glm::mat4 get_mvp( ) const;

    glm::vec3 pos;

    GroupTransformComponent* parent;
};

class GroupTransformComponent : public Component< GroupTransformComponent >
{
public:
    GroupTransformComponent( const char* name );

    void add_child( TransformComponent* tc );

    glm::mat4 get_mvp( ) const;

    void on_detached_component( IComponent* comp ) override;

    void on_detached( Entity* ent ) override;

    basic::Vector< TransformComponent* > child_transform;
};

class RenderComponent : public Component< RenderComponent >
{
public:
    RenderComponent( const char* name );

    void on_attached( Entity* ent ) override;

    void on_detached( Entity* ) override;

    void on_attached_component( IComponent* comp ) override;

    void on_detached_component( IComponent* comp ) override;

    glm::mat4 get_mvp( ) const;

    void apply_material( const glm::mat4& cam );

public:
    Material* material;
    TransformComponent* transform;
    basic::Color color;
    basic::uint32 array_object = 0;
    basic::uint32 vertex_object = 0;
    basic::int32 vertex_elements = 0;
    basic::uint32 index_object = 0;
    basic::int32 index_elements = 0;
};
