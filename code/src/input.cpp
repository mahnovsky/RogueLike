#include "input.hpp"

namespace input
{

InputListener::~InputListener() {}

void InputListener::key_pressed(input::KeyCode code, basic::int16 key) {}

void InputListener::mouse_pressed(input::MouseButton button, basic::int32 x, basic::int32 y) {}

void InputListener::mouse_moved(basic::int32 x, basic::int32 y){}

Input::Input()
{
}

Input::~Input() {}

void Input::key_pressed(input::KeyCode code, basic::int16 key)
{
    for(basic::uint32 i = 0; i < m_listeners.get_size(); ++i)
    {
        InputListener* listener = m_listeners[i];
        ASSERT( listener != nullptr );

        listener->key_pressed( code, key );
    }
}

void Input::mouse_pressed(input::MouseButton button, basic::int32 x, basic::int32 y)
{
    for(basic::uint32 i = 0; i < m_listeners.get_size(); ++i)
    {
        InputListener* listener = m_listeners[i];
        ASSERT( listener != nullptr );

        listener->mouse_pressed( button, x, y );
    }
}

void Input::mouse_moved(basic::int32 x, basic::int32 y)
{
    for(basic::uint32 i = 0; i < m_listeners.get_size(); ++i)
    {
        InputListener* listener = m_listeners[i];
        ASSERT( listener != nullptr );

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

Input *Input::create()
{
    return NEW_OBJ(Input);
}

}
