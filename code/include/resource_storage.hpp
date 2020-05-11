#pragma once

#include "defines.hpp"
#include "object.hpp"
#include "type_registration.hpp"
#include "generic/generic_object_manager.hpp"

class ResourceStorage;

class FileResource : public IGenericObject
{
public:
    FileResource(GenericObjectManager* manager, const char* file);

    virtual ~FileResource();

    virtual bool load(ResourceStorage*) { return false; }

    GenericObjectManager* get_object_manager() { return m_object_manager; }

    std::string_view get_file_name() const { return m_file_name; }
private:
    GenericObjectManager* m_object_manager;
    std::string m_file_name;
};

class ResourceStorage : public IGenericObject
{
public:
    GENERIC_OBJECT_IMPL(ResourceStorage, NS_SYSTEM_TYPE);

    ResourceStorage( EcsManager* ecs, GenericObjectManager* manager );
    ~ResourceStorage( );

    bool add_resource( FileResource* file_resource );

    template < class T, class ... Args >
    T* get_resorce( const char* file, Args ... args )
    {
        T* res = dynamic_cast< T* >( find_resource( file ) );

        if ( !res )
        {
            res = T::create( m_manager, file, args ... );

            if(!add_resource( res ))
            {
				DELETE_OBJ(res);
				res = nullptr;
            }
        }

        return res;
    }

    FileResource* find_resource( const char* file );

private:
    static void update_cached_resources( void* storage );

private:
    GenericObjectManager* m_manager;
    basic::Vector< FileResource* > m_resources;
};
