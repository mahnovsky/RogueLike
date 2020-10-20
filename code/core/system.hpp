#pragma once

namespace core
{
	class SystemManager;

	enum SystemUID
	{
		SUID_EngineSystem,
		SUID_AssetSystem,
		SUID_WidgetSystem,
		SUID_Max
	};

	extern const char* system_names[SUID_Max];

	class ISystem
	{
	public:
		virtual ~ISystem() = default;

		virtual void initialize() = 0;

		virtual void shutdown() = 0;

		virtual void destroy() = 0;

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

		void destroy() override
		{
			DELETE_OBJ(this);
		}

		SystemUID get_uid() const override
		{
			return UID;
		}

		SystemManager& get_system_manager()
		{
			return m_system_manager;
		}

		const char* get_name() const
		{
			return system_names[UID];
		}

		static const SystemUID UID;
	private:
		SystemManager& m_system_manager;
	};

	template <class T, SystemUID uid>
	const SystemUID typename TSystem<T, uid>::UID = uid;
}