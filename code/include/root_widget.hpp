#pragma once

#include "widget.hpp"
#include "input.hpp"

class IEngine;

class RootWidget
	: public Widget
	, public input::InputListener
{
public:
	RootWidget(IEngine* engine, ICamera* ui_camera);
	~RootWidget() override;

	void add_action(const WidgetAction& action);

	void invoke_action(const basic::String& action_name, Widget* w = nullptr);

	void bind_key_action(input::KeyCode code, const basic::String& action);

	const ICamera* get_ui_camera() const;

private:
    void key_pressed(input::KeyCode code, basic::int16 key) override;

private:
	IEngine* m_engine;
	ICamera* m_ui_camera;

    basic::Vector<WidgetAction> m_actions;
	basic::Vector<basic::Pair<input::KeyCode, basic::String>> m_key_actions;
};
