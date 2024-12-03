#pragma once

#include "input.hpp"
#include "system.hpp"


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

	private:
		std::vector<IEventListener*> m_listeners;
	};

}