#pragma once

#include "defines.hpp"
#include "generic_object.hpp"
#include "system_manager.hpp"
#include "event_system.hpp"

namespace core
{
	enum EventTypes
	{
		ET_BASE_EVENT,
		ET_WIDGET_BUTTON,
		ET_INPUT_MOUSE
	};

	using EventId = std::string;
	class BaseEvent : public TGenericObject <BaseEvent, IGenericObject, "BaseEvent">
	{
	public:
		BaseEvent(EventId event_id)
			: m_event_id(std::move(event_id))
		{
		}

		~BaseEvent() override = default;

		const EventId m_event_id;
	};

	class IEventListener
	{
	public:
		virtual ~IEventListener() = default;
		virtual void on_event(BaseEvent* event) = 0;
	};

	namespace detail
	{

		template<class Event>
		class IEventMethodCallInfo
		{
		public:
			virtual ~IEventMethodCallInfo() = default;

			virtual void call(Event* event) = 0;
		};

		template<class Event, class Receiver>
		class EventMethodCallInfo : public IEventMethodCallInfo<Event>
		{
		public:
			using Method = void (Receiver::*)(Event*);

			EventMethodCallInfo(Receiver* receiver, Method method)
				: m_receiver(receiver)
				, m_method(method)
			{
			}

			void call(Event* event) override
			{
				(*m_receiver.*m_method)(event);
			}

		private:
			Receiver* m_receiver;
			Method m_method;
		};
	} // namespace detail

	template<class Event>
	class EventSubscription : public IEventListener
	{
	public:
		EventSubscription(const EventId& id)
			: m_event_id(id)
		{
		}

		~EventSubscription() override
		{
			if (auto system = core::get_system<EventSystem>()) {
				system->unsubscribe_event(this);
			}
		}

		template<class Receiver>
		void bind(Receiver* receiver, void (Receiver::*method)(Event*))
		{
			m_method_info = std::make_unique<detail::EventMethodCallInfo<Event, Receiver>>(receiver, method);
			if (auto system = core::get_system<EventSystem>()) {
				system->subscribe_event(this);
			}
		}

	private:
		void on_event(BaseEvent* event) override
		{
			if (event->get_meta_info()->hash != Event::CLASS_META_INFO.hash) {
				return;
			}

			if (!m_event_id.empty() && event->m_event_id != m_event_id) {
				return;
			}

			if (m_method_info) {
				Event* ev = fast_cast<Event, EventTypes>(event);

				m_method_info->call(ev);
			}
		}

	private:
		EventId m_event_id;
		std::unique_ptr<detail::IEventMethodCallInfo<Event>> m_method_info;
	};
} // namespace core

