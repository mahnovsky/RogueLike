#pragma once

#include "defines.hpp"
#include "object.hpp"

class ResourceStorage;

class FileResource : public Object
{
public:
    FileResource( const char* file );

    virtual ~FileResource();

    virtual bool load( ResourceStorage* ) = 0;
};

class ResourceStorage final
{
public:
    ResourceStorage( );
    ~ResourceStorage();

    bool add_resource( FileResource* file_resource );

    template<class T>
    T* get_resorce( const char* file )
    {
        T* res = dynamic_cast<T*>( find_resource( file ) );

        if( !res )
        {
            res = T::create( file );

            add_resource( res );
        }

        return res;
    }

    FileResource* find_resource(const char* file);

private:
    static void update_cached_resources( void* storage );

private:
    basic::Vector<FileResource*> m_resources;
};
