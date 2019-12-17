#pragma once

#include "defines.hpp"

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
    Left,
    Middle,
    Right
};

class InputListener
{
public:
    virtual ~InputListener() = default;
    virtual void key_pressed(KeyCode code, basic::int16 key);
	virtual void key_released(KeyCode code, basic::int16 key);
	
    virtual void mouse_pressed(MouseButton button, basic::int32 x, basic::int32 y);
    virtual void mouse_moved(basic::int32 x, basic::int32 y);
};

class Input : public InputListener
{
public:
    Input() = default;
    ~Input() override = default;

    void key_pressed(KeyCode code, basic::int16 key) override;
	void key_released(KeyCode code, basic::int16 key) override;

    void mouse_pressed(MouseButton button, basic::int32 x, basic::int32 y) override;
    void mouse_moved(int x, int y) override;

    void add_listener( InputListener* listener );
    void remove_listener( InputListener* listener );

	bool is_key_pressed(input::KeyCode code) const;

    static Input* create();

private:
    basic::Vector<InputListener*> m_listeners;
	bool m_keys_state[static_cast<basic::uint32>(KeyCode::Count)] = { false };
};

}
