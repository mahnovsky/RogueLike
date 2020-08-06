#pragma once

#include "defines.hpp"
#include "type_registration.hpp"
#include "generic/generic_object_manager.hpp"

#include <memory>

class ResourceStorage;

class FileResource : public std::enable_shared_from_this<FileResource>
{
public:
    FileResource(const char* file);

    virtual ~FileResource() = default;

    virtual bool load(ResourceStorage*) { return false; }

    std::string_view get_file_name() const { return m_file_name; }

private:
    std::string m_file_name;
};

using FileResourcePtr = std::shared_ptr<FileResource>;

class ResourceStorage : public IGenericObject
{
public:
    GENERIC_OBJECT_IMPL(ResourceStorage, NS_SYSTEM_TYPE);

    ResourceStorage(EcsManager*);

    bool add_resource( FileResourcePtr file_resource );

    template < class T, class ... Args >
    std::shared_ptr<T> get_resorce( const char* file, Args ... args )
    {
		std::shared_ptr<T> res = std::dynamic_pointer_cast< T >( find_resource( file ) );

        if ( !res )
        {
			res = std::make_shared<T>(file, args...);

			if (!_inner_add_resource(res))
			{
				LOG("Failed to load resource: %s", file);

				return {};
			}
        }

		return res;
    }

    FileResourcePtr find_resource( const char* file );

private:
    static void update_cached_resources( void* storage );

	bool _inner_add_resource(FileResourcePtr resource);

private:
    std::vector< FileResourcePtr > m_resources;
};
