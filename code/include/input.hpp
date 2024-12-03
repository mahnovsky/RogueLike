#pragma once

#include "defines.hpp"
#include "type_info.hpp"

namespace input
{

enum class KeyCode : char
{
    Invalid,
    AaZz,
	A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
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
    CapsLock,

	Count
};

enum MouseButton
{
	None,
    Left,
    Middle,
    Right
};

enum MouseEventType
{
	Pressed,
	Released,
	Moved
};

struct MouseEvent
{
	MouseEventType type;
	MouseButton button;
	int pos_x;
	int pos_y;
};

class InputListener
{
public:
    virtual ~InputListener() = default;
    virtual void key_pressed(KeyCode code, basic::int16 key);
	virtual void key_released(KeyCode code, basic::int16 key);
	
	virtual void on_mouse_event(const MouseEvent& mouse_event) {}
};

class Input : public InputListener
{
public:
    Input() = default;
    ~Input() override = default;

    void key_pressed(KeyCode code, basic::int16 key) override;
	void key_released(KeyCode code, basic::int16 key) override;

	void on_mouse_event(const MouseEvent& mouse_event) override;

    void add_listener( InputListener* listener );
    void remove_listener( InputListener* listener );

	bool is_key_pressed(input::KeyCode code) const;

    static Input* create();

private:
    basic::Vector<InputListener*> m_listeners;
	bool m_keys_state[static_cast<basic::uint32>(KeyCode::Count)] = { false };
};

}
