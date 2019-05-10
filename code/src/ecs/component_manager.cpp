#include "component_manager.hpp"
#include "entity.hpp"

ComponentManager::ComponentManager( )
    : m_storages( )
{
    m_storages.resize( components_count );
    for ( basic::uint32 i = 0; i < components_count; ++i )
    {
        m_storages[ i ] = nullptr;
    }
}

ComponentManager::~ComponentManager( )
{
    for ( auto& s : m_storages )
    {
        DELETE_OBJ( s );
    }
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
