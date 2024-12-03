#pragma once

#include "memory.hpp"

#include <stdint.h>

namespace core
{
	class SystemManager;
	class IGlobalContext;

	enum class SystemUID : uint32_t
	{
		SUID_CoreSystem,
		SUID_AssetSystem,
		SUID_WidgetSystem,
		SUID_FileSystem,
		SUID_EventSystem,
		SUID_Max
	};

	extern const char* system_names[static_cast<uint32_t>(SystemUID::SUID_Max)];

	class ISystem
	{
	public:
		virtual ~ISystem() = default;

		virtual void initialize(IGlobalContext* context) = 0;

		virtual void update() {}

		virtual void draw() {}

		virtual void shutdown() = 0;

		virtual SystemUID get_uid() const = 0;

		virtual SystemManager& get_system_manager() = 0;

		virtual const char* get_name() const = 0;
	};

	template <class T, SystemUID uid>
	class TSystem : public ISystem
	{
	public:
		TSystem(SystemManager& manager)
			:m_system_manager(manager)
		{
		}

		SystemUID get_uid() const override
		{
			return UID;
		}

		SystemManager& get_system_manager() override
		{
			return m_system_manager;
		}

		const char* get_name() const override
		{
			return system_names[static_cast<uint32_t>(UID)];
		}

		static const SystemUID UID;
	private:
		SystemManager& m_system_manager;
	};

	template <class T, SystemUID uid>
	const SystemUID typename TSystem<T, uid>::UID = uid;
}