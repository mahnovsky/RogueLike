#include "resource_storage.hpp"

#include "timer_manager.hpp"

FileResource::FileResource(GenericObjectManager* manager, const char* file )
    :m_object_manager(manager)
    ,m_file_name(file)
{
}

FileResource::~FileResource( )
{
}

ResourceStorage::ResourceStorage(EcsManager* ecs, GenericObjectManager* manager )
    : m_manager( manager )
{
    TimerManager& timer_manager = TimerManager::get( );

    timer_manager.add( 2.f, &update_cached_resources, this, -1 );
}

ResourceStorage::~ResourceStorage( )
{
    for ( auto obj : m_resources )
    {
        obj->deref( );
    }
    m_resources.clear( );
}

bool
ResourceStorage::add_resource( FileResource* file_resource )
{
    if ( !find_resource( file_resource->get_name( ).c_str( ) ) && file_resource->load(this) )
    {
        m_resources.push( file_resource );

        file_resource->ref( );

        return true;
    }

    return false;
}

FileResource*
ResourceStorage::find_resource( const char* file )
{
    FileResource* result = nullptr;

    for ( basic::uint32 i = 0; i < m_resources.get_size( ); ++i )
    {
        if ( m_resources[ i ]->get_name( ) == file )
        {
            result = m_resources[ i ];
            break;
        }
    }

    return result;
}

void
ResourceStorage::update_cached_resources( void* storage )
{
    ResourceStorage* rs = static_cast< ResourceStorage* >( storage );

    for ( basic::uint32 i = 0; i < rs->m_resources.get_size( ); ++i )
    {
        if ( rs->m_resources[ i ]->get_refs( ) == 1 )
        {
            rs->m_resources[ i ]->deref( );

            rs->m_resources.swap_remove( i );
        }
    }
}
