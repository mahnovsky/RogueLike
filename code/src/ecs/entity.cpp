#include "entity.hpp"

Entity::Entity( EntityID id, EntityComponentSystem* ecs )
    : m_uid( id )
    , m_ecs( ecs )
{
}

Entity::~Entity( )
{
    m_ecs->destroy( this );
}

EntityID
Entity::get_uid( ) const
{
    return m_uid;
}

void
Entity::on_destroy( )
{
    for ( auto id : m_components )
    {
        m_ecs->emit( this, id, ComponentAction::Detached );
    }
}
