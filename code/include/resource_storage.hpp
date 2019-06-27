#pragma once

#include "defines.hpp"
#include "object.hpp"

class ResourceStorage;

class FileResource : public SharedObject
{
public:
    FileResource( ObjectManager* manager, SharedObjectType type, const char* file );

    virtual ~FileResource( );

    virtual bool load( ResourceStorage* ) = 0;
};

class ResourceStorage final
{
public:
    ResourceStorage( ObjectManager* manager );
    ~ResourceStorage( );

    bool add_resource( FileResource* file_resource );

    template < class T >
    T*
    get_resorce( const char* file )
    {
        T* res = dynamic_cast< T* >( find_resource( file ) );

        if ( !res )
        {
            res = T::create( m_manager, file );

            add_resource( res );
        }

        return res;
    }

    FileResource* find_resource( const char* file );

private:
    static void update_cached_resources( void* storage );

private:
    ObjectManager* m_manager;
    basic::Vector< FileResource* > m_resources;
};
