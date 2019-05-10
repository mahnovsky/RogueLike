#include "component_manager.hpp"
#include "entity.hpp"

ComponentManager::ComponentManager( )
    : m_storages( )
{
    m_storages.resize( components_count );
}

ComponentManager::~ComponentManager( )
{
}

IComponent*
ComponentManager::create_component( const char* name )
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
