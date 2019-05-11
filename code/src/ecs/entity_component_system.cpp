#include "entity_component_system.hpp"
#include "entity.hpp"

EntityComponentSystem::EntityComponentSystem( )
    :m_id_counter(0)
	,m_entities( )
	,m_storages( )
{
    m_storages.resize( components_count );
    for ( basic::uint32 i = 0; i < components_count; ++i )
    {
        m_storages[ i ] = nullptr;
    }
}

EntityComponentSystem::~EntityComponentSystem( )
{
    for ( auto& s : m_storages )
    {
        DELETE_OBJ( s );
    }
}

IComponent*
EntityComponentSystem::create_component( const char* name )
{
    for ( const auto& s : m_storages )
    {
        if ( s->get_name( ) == name )
        {
            return s->create( );
        }
    }
    LOG( "Failed create component by name, factory not found" );

    return nullptr;
}

Entity* EntityComponentSystem::create()
{
	Entity* ent = NEW_OBJ(Entity, m_id_counter);

	++m_id_counter;

	m_entities.push(ent);

	return ent;
}

void EntityComponentSystem::destroy(Entity* ent)
{
}

void EntityComponentSystem::update(float dt)
{
}

