#include "entity.hpp"

Entity::Entity( EntityID id )
    : m_entity_id( id )
	, m_components( )
{
}

Entity::~Entity( )
{
}

void
Entity::initialize( )
{
}

void
Entity::add_component( IComponent* component )
{
    auto id = component->get_type_id( );

    if ( !find_component( id ) )
    {
        m_components.push( component );

        component->on_attached( this );
    }
}

IComponent*
Entity::find_component( ComponentTypeID id )
{
    basic::uint32 index = 0;
    if ( m_components.find_if(
                 index, [id]( const IComponent* comp ) { return comp->get_type_id( ) == id; } ) )
    {
        return m_components[ index ];
    }

    return nullptr;
}
