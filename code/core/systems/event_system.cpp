#include "event_system.hpp"

#include "engine.hpp"
#include "render.hpp"
#include "event.hpp"

using namespace input;

namespace core
{
	EventSystem::EventSystem(SystemManager& manager)
		: TSystem(manager)
	{
	}

	void EventSystem::initialize(IGlobalContext* context)
	{
	}

	void EventSystem::shutdown()
	{
	}

	void EventSystem::subscribe_event(IEventListener* event_listener)
	{
		stdext::push_unique(m_listeners, event_listener);
	}

	void EventSystem::unsubscribe_event(IEventListener* event_listener)
	{
		stdext::remove(m_listeners, event_listener);
	}

	void EventSystem::raise_event(BaseEvent* event) const
	{
		for (auto* listener : m_listeners) {
			listener->on_event(event);
		}
	}
}