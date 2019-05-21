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

    if ( m_components.get_size( ) < id )
    {
        basic::Vector< IComponent* > com{m_components};
        m_components.resize( id + 1 );
        for ( basic::uint32 i = 0; i < com.get_size( ); ++i )
        {
            m_components[ i ] = com[ i ];
        }
    }

    if ( m_components[ id ] )
    {
        m_components[ id ]->on_detached( this );
    }

    m_components[ id ] = component;

    component->on_attached( this );
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
