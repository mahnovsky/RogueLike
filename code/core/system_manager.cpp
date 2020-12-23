#include "system_manager.hpp"

#include "basic/debug.hpp"

namespace core
{
#define AS_STRING(SUID) #SUID

	const char* system_names[SUID_Max] =
	{
		AS_STRING(SUID_EngineSystem),
		AS_STRING(SUID_AssetSystem),
		AS_STRING(SUID_WidgetSystem)
	};

	SystemManager::SystemManager()
		:m_state(LifecycleState::Created)
		,m_systems()
	{
		LOG("SystemManager created");
	}

	SystemManager::~SystemManager()
	{
		LOG("Destroy all systems");
		ASSERT(m_state == LifecycleState::Shotdowned);
		for (ISystem* system : m_systems)
		{
			system->destroy();
		}
		m_state = LifecycleState::Destroyed;
	}

	void SystemManager::add_system(ISystem* system)
	{
		LOG("Added system");
		ASSERT(m_state == LifecycleState::Created);
		SystemUID uid = system->get_uid();
		const size_t need_size = (uid + 1);
		if (m_systems.size() < need_size)
		{
			m_systems.resize(need_size);
		}

		m_systems[uid] = system;
	}

	ISystem* SystemManager::get_system(SystemUID uid)
	{
		return m_systems[uid];
	}

	void SystemManager::initialize()
	{
		LOG("Initialize all systems");
		ASSERT(m_state == LifecycleState::Created);

		for (ISystem* system : m_systems)
		{
			if (system)
			{
				system->initialize();
			}
		}
		m_state = LifecycleState::Initialized;
	}

	void SystemManager::shutdown()
	{
		LOG("Shutdown all systems");
		ASSERT(m_state == LifecycleState::Initialized);
		for (ISystem* system : m_systems)
		{
			if (system)
			{
				system->shutdown();
			}
		}
		m_state = LifecycleState::Shotdowned;
	}

	LifecycleState SystemManager::get_lifecycle_state() const
	{
		return m_state;
	}

}
