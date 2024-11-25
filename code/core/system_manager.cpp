#include "system_manager.hpp"

#include "debug.hpp"

namespace core
{
#define AS_STRING(SUID) #SUID

	const char* system_names[static_cast<uint32_t>(SystemUID::SUID_Max)] =
	{
		AS_STRING(SUID_EngineSystem),
		AS_STRING(SUID_AssetSystem),
		AS_STRING(SUID_WidgetSystem)
	};

	SystemManager::SystemManager(IGlobalContext* context)
		:_context(context)
		,_systems()
	{
		LOG("SystemManager created");
	}

	SystemManager::~SystemManager()
	{
	}

	void SystemManager::add_system(ISystem* system)
	{
		SystemUID uid = system->get_uid();
		const size_t need_size = (static_cast<size_t>(uid) + 1u);
		if (_systems.size() < need_size)
		{
			_systems.resize(need_size);
		}
		assert(_systems[static_cast<size_t>(uid)] == nullptr);
		_systems[static_cast<size_t>(uid)] = system;

		_initialize_systems.push_back(system);
	}

	ISystem* SystemManager::get_system(SystemUID uid)
	{
		return _systems[static_cast<uint32_t>(uid)];
	}

	void SystemManager::update()
	{
		if (!_initialize_systems.empty())
		{
			for (auto system : _initialize_systems)
			{
				system->initialize(_context);
			}
			_initialize_systems.clear();
		}
	}

	void SystemManager::shutdown()
	{
		LOG("Shutdown all systems");
		for (ISystem* system : _systems)
		{
			if (system)
			{
				system->shutdown();

				basic::Memory::Delete(system);
			}
		}
		_systems.clear();
	}
}
