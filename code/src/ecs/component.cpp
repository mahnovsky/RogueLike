#include "component.hpp"
#include "entity.hpp"

ComponentTypeID components_count;

TransformComponent::TransformComponent( const char* name )
    : Component< TransformComponent >( name )
{
}

RenderComponent::RenderComponent( const char* name )
    : Component< RenderComponent >( name )
{
}

void
RenderComponent::on_attached( Entity* ent )
{
    Component< RenderComponent >::on_attached( ent );

    transform = ent->get_component< TransformComponent >( );
}

void
RenderComponent::on_detached( Entity* )
{
    transform = nullptr;
}

void
RenderComponent::on_attached_component( IComponent* comp )
{
    if ( comp->get_type_id( ) == TransformComponent::_type_id )
    {
        transform = static_cast< TransformComponent* >( static_cast< void* >( comp ) );
    }
}

void
RenderComponent::on_dettached_component( IComponent* comp )
{
    if ( comp->get_type_id( ) == TransformComponent::_type_id )
    {
        transform = nullptr;
    }
}
