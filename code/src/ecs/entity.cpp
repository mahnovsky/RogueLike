#include "entity.hpp"

Entity::Entity( EntityID id )
    : m_entity_id( id )
    , m_components( )
{
    m_components.resize( components_count );
    m_components.fill( nullptr );
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

    if ( m_components.get_size( ) < id )
    {
        basic::Vector< IComponent* > com{m_components};
        m_components.resize( id + 1 );
        for ( basic::uint32 i = 0; i < com.get_size( ); ++i )
        {
            m_components[ i ] = com[ i ];
        }
    }

    ASSERT( m_components.get_size( ) > id );

    if ( m_components[ id ] )
    {
        m_components[ id ]->on_detached( this );

        component_event_broadcast( m_components[ id ], false );
    }

    m_components[ id ] = component;

    component->on_attached( this );
    component_event_broadcast( component, true );
}

IComponent*
Entity::find_component( ComponentTypeID id )
{
    if ( id < m_components.get_size( ) )
    {
        return m_components[ id ];
    }

    return nullptr;
}

EntityID
Entity::get_id( ) const
{
    return m_entity_id;
}

void
Entity::on_destroy( )
{
    m_components.clear( );
}

void
Entity::component_event_broadcast( IComponent* comp, bool attached )
{
    for ( IComponent* c : m_components )
    {
        if ( !c )
        {
            continue;
        }
        if ( attached )
        {
            c->on_attached_component( comp );
        }
        else
        {
            c->on_dettached_component( comp );
        }
    }
}
