#include "component.hpp"
#include "entity.hpp"

#include "material.hpp"
#include "render_common.hpp"

ComponentTypeID components_count;

TransformComponent::TransformComponent( const char* name )
    : Component< TransformComponent >( name )
{
}

glm::mat4
TransformComponent::get_mvp( ) const
{
    if ( parent )
    {
    }
    return glm::mat4{1.f};
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
RenderComponent::on_detached_component( IComponent* comp )
{
    if ( comp->get_type_id( ) == TransformComponent::_type_id )
    {
        transform = nullptr;
    }
}

glm::mat4
RenderComponent::get_mvp( ) const
{
    if ( transform )
    {
        return transform->get_mvp( );
    }
    return glm::mat4( 1.f );
}

void
RenderComponent::apply_material( const glm::mat4& cam )
{
    material->enable( );
    material->set_uniform( "mvp", cam * get_mvp( ) );
    material->set_uniform( "Color", color );
}

GroupTransformComponent::GroupTransformComponent( const char* name )
    : Component< GroupTransformComponent >( name )
{
}

void
GroupTransformComponent::add_child( TransformComponent* tc )
{
    if ( !child_transform.is_contains( tc ) )
    {
        child_transform.push( tc );
        tc->parent = this;
        tc->add_listener( this );
    }
}

glm::mat4
GroupTransformComponent::get_mvp( ) const
{
    const Entity* ent = get_entity( );
    const TransformComponent* tc = nullptr;
    if ( ent && ( tc = ent->get_component< TransformComponent >( ) ) )
    {
        return tc->get_mvp( );
    }
    return glm::mat4( 1.f );
}

void
GroupTransformComponent::on_detached( Entity* )
{
    for ( auto c : child_transform )
    {
        c->remove_listener( this );
        c->parent = nullptr;
    }
    child_transform.clear( );
}

void
GroupTransformComponent::on_detached_component( IComponent* comp )
{
    if ( TransformComponent::_type_id == comp->get_type_id( ) )
    {
        child_transform.remove_by_value( fast_cast< TransformComponent >( comp ) );
    }
}
