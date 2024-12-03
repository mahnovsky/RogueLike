#include "system_manager.hpp"

#include "debug.hpp"

namespace core
{
#define AS_STRING(SUID) #SUID

	const char* system_names[static_cast<uint32_t>(SystemUID::SUID_Max)] =
	{
		AS_STRING(SUID_EngineSystem),
		AS_STRING(SUID_AssetSystem),
		AS_STRING(SUID_WidgetSystem),
		AS_STRING(SUID_EventSystem)
	};

	SystemManager::SystemManager(IGlobalContext* context)
		:_context(context)
		,_systems()
	{
		LOG("SystemManager created");
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

	ISystem* SystemManager::get_system(SystemUID uid) const
	{
		return _systems[static_cast<uint32_t>(uid)];
	}

	void SystemManager::initialize_systems()
	{
		if (_initialize_systems.empty())
		{
			return;
		}

		for (const auto system : _initialize_systems)
		{
			system->initialize(_context);
		}
		_initialize_systems.clear();
	}

	void SystemManager::update()
	{
		initialize_systems();

		for (const auto system : _systems)
		{
			system->update();
		}
	}

	void SystemManager::draw()
	{
		for (const auto system : _systems)
		{
			system->draw();
		}
	}

	void SystemManager::shutdown()
	{
		LOG("Shutdown all systems");
		while (!_systems.empty())
		{
			if (const auto system = _systems.back())
			{
				system->shutdown();

				basic::Memory::Delete(system);
			}
			_systems.pop_back();
		}
		_systems.clear();
	}
}
