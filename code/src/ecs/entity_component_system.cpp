#include "entity_component_system.hpp"
#include "entity.hpp"

EntityComponentSystem::EntityComponentSystem( )
    : m_id_counter( 0 )
    , m_entities( )
    , m_storages( )
{
    m_storages.resize( components_count );
    for ( basic::uint32 i = 0; i < components_count; ++i )
    {
        m_storages[ i ] = nullptr;
    }
}

EntityComponentSystem::~EntityComponentSystem( )
{
    for ( auto e : m_entities )
    {
        DELETE_OBJ( e );
    }
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

Entity*
EntityComponentSystem::create( )
{
    if ( !m_destroyed.is_empty( ) )
    {
        auto back = m_destroyed.back( );
        m_entities.push( back );
        m_destroyed.pop( );
        return back;
    }

    Entity* ent = NEW_OBJ( Entity, m_id_counter );

    ++m_id_counter;

    m_entities.push( ent );

    return ent;
}

void
EntityComponentSystem::destroy( Entity* ent )
{
    basic::uint32 index = 0;

    if ( m_entities.find_first( index, ent ) )
    {
        m_entities.swap_remove( index );
        m_destroyed.push( ent );
    }
}

void
EntityComponentSystem::update( float )
{
    for ( auto s : m_systems )
    {
        s->update( this );
    }
}
