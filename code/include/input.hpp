#pragma once

#include "defines.hpp"

namespace input
{

enum class KeyCode
{
    Invalid,
    AaZz,
    Space,
    Enter,
    LeftCtrl,
    RightCtrl,
    LeftAlt,
    RightAlt,
    LeftShift,
    RightShift,
    Tab,
    Esc,
    Backspace,
    CapsLock
};

enum MouseButton
{
    Left,
    Middle,
    Right
};

class InputListener
{
public:
    virtual ~InputListener();
    virtual void key_pressed(KeyCode code, basic::int16 key);
    virtual void mouse_pressed(MouseButton button, basic::int32 x, basic::int32 y);
    virtual void mouse_moved(basic::int32 x, basic::int32 y);
};

class Input : public InputListener
{
public:
    Input();
    ~Input() override;

    void key_pressed(KeyCode code, basic::int16 key);
    void mouse_pressed(MouseButton button, basic::int32 x, basic::int32 y);
    void mouse_moved(int x, int y);

    void add_listener( InputListener* listener );
    void remove_listener( InputListener* listener );

    static Input* create();

private:
    basic::Vector<InputListener*> m_listeners;
};

}
