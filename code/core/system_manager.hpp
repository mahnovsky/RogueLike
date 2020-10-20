#pragma once

#include <vector>
#include "system.hpp"

namespace core
{
	enum class LifecycleState
	{
		Created,
		Initialized,
		Shotdowned,
		Destroyed
	};

	class SystemManager
	{
	public:
		SystemManager();
		SystemManager(const SystemManager&) = delete;
		SystemManager(SystemManager&&) = delete;
		~SystemManager();

		template <class T>
		void add_system()
		{
			T* system = NEW_OBJ(T, *this);
			add_system(system);
		}

		void add_system(ISystem* system);

		ISystem* get_system(SystemUID uid);

		template <class T>
		T* get_system()
		{
			ISystem* system = get_system(T::UID);
			if (system != nullptr)
			{
				return dynamic_cast<T*>(system);
			}
			return nullptr;
		}

		void initialize();

		void shutdown();

	private:
		LifecycleState m_state;
		std::vector<ISystem*> m_systems;
	};
}