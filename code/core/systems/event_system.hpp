#pragma once

#include "input.hpp"
#include "system.hpp"
#include "pool.hpp"

namespace core
{
	class IEngine;
	class BaseEvent;
	class IEventListener;

	class EventSystem
		: public TSystem<EventSystem, SystemUID::SUID_EventSystem>
	{
	public:
		EventSystem(SystemManager& manager);
		~EventSystem() override = default;

		void initialize(IGlobalContext* context) override;

		void shutdown() override;

		void subscribe_event(IEventListener* event_listener);
		
		void unsubscribe_event(IEventListener* event_listener);

		void raise_event(BaseEvent* event) const;

		template<class Event, class ... Args>
		void raise_event(Args ... args) const
		{
			static Pool<Event> event_pool;

			auto ev = event_pool.alloc(args...);
			raise_event(ev);

			event_pool.free(ev);
		}

	private:
		std::vector<IEventListener*> m_listeners;
	};

}