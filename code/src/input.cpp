#include "input.hpp"

namespace input
{

void InputListener::key_pressed(input::KeyCode , basic::int16 ) {}

void InputListener::key_released(input::KeyCode, basic::int16) {}

void InputListener::mouse_pressed(input::MouseButton , basic::int32 , basic::int32 ) {}

void InputListener::mouse_moved(basic::int32 , basic::int32 ){}


void Input::key_pressed(input::KeyCode code, basic::int16 key)
{
	m_keys_state[static_cast<basic::uint32>(code)] = true;
    for(InputListener* listener : m_listeners)
    {
        listener->key_pressed( code, key );
    }
}

void Input::key_released(KeyCode code, basic::int16 key)
{
	m_keys_state[static_cast<basic::uint32>(code)] = false;
	for (InputListener* listener : m_listeners)
	{
		listener->key_released(code, key);
	}
}

void Input::mouse_pressed(input::MouseButton button, basic::int32 x, basic::int32 y)
{
    for(InputListener* listener : m_listeners)
    {
        listener->mouse_pressed( button, x, y );
    }
}

void Input::mouse_moved(basic::int32 x, basic::int32 y)
{
    for(InputListener* listener : m_listeners)
    {
        listener->mouse_moved( x, y );
    }
}

void Input::add_listener(InputListener *listener)
{
    m_listeners.push( listener );
}

void Input::remove_listener(InputListener *listener)
{
    m_listeners.remove_by_value( listener );
}

bool Input::is_key_pressed(input::KeyCode code) const
{
	return m_keys_state[static_cast<basic::uint32>(code)];
}

Input *Input::create()
{
    return NEW_OBJ(Input);
}

}
