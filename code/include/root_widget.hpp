#pragma once

#include "widget.hpp"
#include "input.hpp"

class RootWidget
        : public Widget
        , public input::InputListener
{
public:
    RootWidget(Engine* engine, ObjectManager* manager);
    ~RootWidget() override;

private:
    void key_pressed(input::KeyCode code, basic::int16 key) override;
    void mouse_pressed(input::MouseButton button, basic::int32 x, basic::int32 y) override;
    void mouse_moved(basic::int32 x, basic::int32 y) override;

private:
    Engine* m_engine;
};
