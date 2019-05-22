#pragma once

#include "defines.hpp"

using ComponentTypeID = basic::uint32;

extern ComponentTypeID components_count;

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

    virtual void on_attached( Entity* entity ) = 0;

    virtual void on_detached( Entity* entity ) = 0;

    virtual void on_attached_component( IComponent* comp ) = 0;

    virtual void on_dettached_component( IComponent* comp ) = 0;
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
    on_dettached_component( IComponent* ) override
    {
    }

private:
    Entity* m_entity;
    basic::String m_name;
};

template < class T >
const ComponentTypeID Component< T >::_type_id = IComponent::type_id< T >( );

class TransformComponent : public Component< TransformComponent >
{
public:
    TransformComponent( const char* name );

    glm::vec3 pos;
};

class RenderComponent : public Component< RenderComponent >
{
public:
    RenderComponent( const char* name );

    void on_attached( Entity* ent ) override;

    void on_detached( Entity* ) override;

    void on_attached_component( IComponent* comp ) override;

    void on_dettached_component( IComponent* comp ) override;

private:
    Material* material;
    TransformComponent* transform;
};
