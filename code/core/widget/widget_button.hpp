#pragma once

#include "widget.hpp"
#include "pool.hpp"
#include "event.hpp"

enum class ButtonEventType
{
	Hovered,
	Pressed,
	Released
};

//REGISTRY_TYPE(ButtonEvent, core::ET_WIDGET_BUTTON, core::EventTypes)
class ButtonEvent : public TGenericObject<ButtonEvent, core::BaseEvent, "ButtonEvent">
{
public:

	ButtonEvent(const core::EventId& event_id, ButtonEventType event_type)
		: Super(event_id)
		,m_event_type(event_type)
	{}

	const ButtonEventType m_event_type;
};


class WidgetButton : public Widget
{
public:
	WidgetButton(core::WidgetSystem* widget);
	~WidgetButton() override = default;

	void initialize();

	void set_texture(const std::string& texture_id);

	void set_press_event_id(const core::EventId& event_id);

	void on_mouse_pressed(input::MouseButton btn, basic::int32 x, basic::int32 y) override;

	void draw(IRender* render) override;

private:
	core::EventId m_press_event;

	IRenderObject* m_view;
	std::string m_texture_name;
};
