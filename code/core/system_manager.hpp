#pragma once

#include <vector>
#include "system.hpp"

namespace core
{
	class IGlobalContext;

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
		SystemManager(IGlobalContext* context);
		SystemManager(const SystemManager&) = delete;
		SystemManager(SystemManager&&) = delete;
		~SystemManager() = default;

		template <class T>
		void add_system()
		{
			T* system = NEW_OBJ(T, *this);
			add_system(system);
		}

		void add_system(ISystem* system);

		ISystem* get_system(SystemUID uid) const;

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

		void initialize_systems();

		void update();

		void draw();

		void shutdown();

	private:
		IGlobalContext* _context;
		std::vector<ISystem*> _initialize_systems;
		std::vector<ISystem*> _systems;
	};
	
	template<class T>
	T* get_system()
	{
		if (IGlobalContext* context = IGlobalContext::GetInstance()) {
			if (const auto smanager = context->get_system_manager()) {
				return smanager->get_system<T>();
			}
		}

		return nullptr;
	}
}