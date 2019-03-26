#include "input.hpp"

namespace input
{

InputListener::~InputListener() {}

void InputListener::key_pressed(input::KeyCode , basic::int16 ) {}

void InputListener::mouse_pressed(input::MouseButton , basic::int32 , basic::int32 ) {}

void InputListener::mouse_moved(basic::int32 , basic::int32 ){}

Input::Input()
{
}

Input::~Input() {}

void Input::key_pressed(input::KeyCode code, basic::int16 key)
{
    for(InputListener* listener : m_listeners)
    {
        listener->key_pressed( code, key );
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

Input *Input::create()
{
    return NEW_OBJ(Input);
}

}
