#pragma once

#include "widget.hpp"
#include "input.hpp"

class RootWidget
        : public Widget
        , public input::InputListener
{
public:
    RootWidget(Engine* engine, GenericObjectManager* manager);
    ~RootWidget() override;

    void add_action( const WidgetAction& action );

    void invoke_action(const basic::String& action_name, Widget* w = nullptr);

	void bind_key_action(input::KeyCode code, const basic::String& action);

private:
    void key_pressed(input::KeyCode code, basic::int16 key) override;


private:
    Engine* m_engine;
    basic::Vector<WidgetAction> m_actions;
	basic::Vector<basic::Pair<input::KeyCode, basic::String>> m_key_actions;
};
