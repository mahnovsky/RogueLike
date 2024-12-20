#include "resource_storage.hpp"

#include "system_manager.hpp"
#include "timer_manager.hpp"

#include <algorithm>

namespace core
{
	FileResource::FileResource(const char* file)
		: m_file_name(file)
	{
	}

	ResourceStorage::ResourceStorage(SystemManager& manager)
		: TSystem< ResourceStorage, SystemUID::SUID_AssetSystem>(manager)
	{
		TimerManager& timer_manager = TimerManager::get();

		m_update_timer = timer_manager.create_timer(std::chrono::seconds { 2 }, &update_cached_resources, this, -1);
	}

	void ResourceStorage::initialize(IGlobalContext* context)
	{
		_context = context;
	}

	void ResourceStorage::shutdown()
	{
		update_cached_resources(this);
	}

	bool ResourceStorage::add_resource(FileResourcePtr file_resource)
	{
		auto file_name = file_resource->get_file_name();
		ASSERT(!file_name.empty());

		if (!find_resource(file_name.data()) && _inner_add_resource(file_resource))
		{
			return true;
		}

		return false;
	}

	FileResourcePtr ResourceStorage::find_resource(const char* file)
	{
		FileResource* result = nullptr;

		auto it = std::find_if(m_resources.begin(), m_resources.end(), [file](const FileResourcePtr res) {
				return res->get_file_name() == file;
			});

		if (it != m_resources.end())
		{
			return *it;
		}

		return nullptr;
	}

	void ResourceStorage::update_cached_resources(void* storage)
	{
		auto rs = static_cast<ResourceStorage*>(storage);

		const auto it = std::remove_if(rs->m_resources.begin(), rs->m_resources.end(), [](FileResourcePtr res)
			{
				return res.use_count() == 1;
			});

		rs->m_resources.erase(it, rs->m_resources.end());
	}

	bool ResourceStorage::_inner_add_resource(FileResourcePtr resource)
	{
		const bool is_loaded = resource->load(this);
		if (is_loaded)
		{
			m_resources.push_back(resource);

			LOG("Resource \"%s\" loaded successfully", resource->get_file_name().data());
		}
		return is_loaded;
	}

}