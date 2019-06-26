#include "entity_component_system.hpp"
#include "entity.hpp"

EntityComponentSystem::EntityComponentSystem( )
    : m_id_counter( 0 )
    , m_entities( )
    , m_storages( )
{
    m_storages.push( nullptr );
}

EntityComponentSystem::~EntityComponentSystem( )
{
    auto ents = m_entities;
    DELETE_ARR_OBJ( ents );
    DELETE_ARR_OBJ( m_storages );
    DELETE_ARR_OBJ( m_systems );
}

void*
EntityComponentSystem::create_component( const char* name )
{
    basic::uint32 index;
    if ( m_storages.find_if( index, [name]( const IComponentStorage* storage ) {
             return storage->get_name( ) == name;
         } ) )
    {
        return m_storages[ index ]->create( );
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

    Entity* ent = NEW_OBJ( Entity, m_id_counter, this );

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
        ent->on_destroy( );
        m_entities.swap_remove( index );
        m_destroyed.push( ent );
    }
}

void
EntityComponentSystem::start( )
{
    for ( auto s : m_systems )
    {
        s->start( );
    }
}

void
EntityComponentSystem::update( float dt )
{
    for ( auto s : m_systems )
    {
        s->update( dt );
    }
}

void
EntityComponentSystem::subscribe( basic::uint32 component_id, ISystem* system )
{
    basic::Vector< ISystem* > subs;

    if ( m_subscribers.find( component_id, subs ) )
    {
        basic::uint32 index;
        if ( !subs.find_first( index, system ) )
        {
            subs.push( system );
            m_subscribers.insert( component_id, subs );
        }
    }
    else
    {
        subs.push( system );
        m_subscribers.insert( component_id, std::move( subs ) );
    }
}

void
EntityComponentSystem::unsubscribe( basic::uint32 component_id, ISystem* system )
{
    basic::Vector< ISystem* > subs;

    if ( m_subscribers.find( component_id, subs ) )
    {
        basic::uint32 index;
        if ( subs.find_first( index, system ) )
        {
            if ( subs.get_size( ) == 1 )
            {
                m_subscribers.remove( component_id );
            }
            else
            {
                subs.swap_remove( index );
                m_subscribers.insert( component_id, subs );
            }
        }
    }
}

void
EntityComponentSystem::emit( Entity* ent, basic::uint32 component_id, ComponentAction act )
{
    auto it = m_subscribers.find( component_id );

    if ( it != m_subscribers.end( ) )
    {
        auto systems = it->value;
        for ( auto system : systems )
        {
            system->on_component_event( ent, component_id, act );
        }
    }
}

void
EntityComponentSystem::broadcast( Entity* ent, basic::uint32 component_id, ComponentAction act )
{
    for ( auto s : m_systems )
    {
        s->on_component_event( ent, component_id, act );
    }
}
