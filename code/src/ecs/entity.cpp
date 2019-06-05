#include "entity.hpp"

Entity::Entity( EntityID id, EntityComponentSystem* ecs )
    : m_uid( id )
    , m_ecs( ecs )
{
}

Entity::~Entity( )
{
}

EntityID
Entity::get_id( ) const
{
    return m_uid;
}

void
Entity::on_destroy( )
{
}
