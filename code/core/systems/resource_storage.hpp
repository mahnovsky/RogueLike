#pragma once

#include "defines.hpp"
#include "system.hpp"
#include "timer_manager.hpp"

#include <memory>

namespace core
{

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

	class ResourceStorage 
		: public TSystem< ResourceStorage, SystemUID::SUID_AssetSystem>
	{
	public:
		ResourceStorage(SystemManager& manager);


		void initialize(IGlobalContext* context) override;

		void shutdown() override;

		bool add_resource(FileResourcePtr file_resource);

		template < class T, class ... Args >
		std::shared_ptr<T> get_resource(const char* file, Args ... args)
		{
			std::shared_ptr<T> res = std::dynamic_pointer_cast<T>(find_resource(file));

			if (!res)
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

		FileResourcePtr find_resource(const char* file);

	private:
		static void update_cached_resources(void* storage);

		bool _inner_add_resource(FileResourcePtr resource);

	private:
		IGlobalContext* _context;
		std::vector< FileResourcePtr > m_resources;
		TimerInstancePtr m_update_timer;
	};
}
