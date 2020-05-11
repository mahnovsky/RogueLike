#include "root_widget.hpp"

#include "engine.hpp"
#include "camera.hpp"

using namespace input;

RootWidget::RootWidget(Engine *engine, GenericObjectManager* manager)
    :Widget (manager)
    ,m_engine(engine)
{
    m_engine->get_input()->add_listener(this);
}

RootWidget::~RootWidget()
{
    m_engine->get_input()->remove_listener(this);
}

void RootWidget::add_action(const WidgetAction &action)
{
    m_actions.push(action);
}

void RootWidget::invoke_action(const basic::String &action_name, Widget *w)
{
    GENERATE_COMP(ActionComp, WidgetAction, basic::String, name);

    basic::uint32 index = 0;
    if( m_actions.find_if(index, ActionComp(action_name)) )
    {
        WidgetAction& wa = m_actions[index];
        LOG("ivoked widget action %s", wa.name.get_cstr());
        wa.callback( w, wa.user_data );
    }
}

void RootWidget::bind_key_action(input::KeyCode code, const basic::String& action)
{
	m_key_actions.push({ code, action });
}

void RootWidget::key_pressed(KeyCode code, basic::int16 key)
{
	for(auto& it : m_key_actions)
	{
		if(it.key == code)
		{
			invoke_action(it.value, this);
		}
	}

    on_key_pressed(code, key);
}

void RootWidget::mouse_pressed(MouseButton button, basic::int32 x, basic::int32 y)
{   
    on_mouse_pressed( button, x, y );
}

void RootWidget::mouse_moved(basic::int32 x, basic::int32 y)
{
    on_mouse_move(x, y);
}
