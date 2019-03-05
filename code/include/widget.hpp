#pragma once

#include "defines.hpp"

#include "object.hpp"
#include "input.hpp"

struct Rect
{
    glm::vec2 left_top;
    glm::vec2 right_bottom;

    Rect()
        :left_top()
        ,right_bottom()
    {}

    Rect( const glm::vec2& lt, const glm::vec2& tb )
        :left_top(lt)
        ,right_bottom(tb)
    {}

    void update(const glm::vec2& pos, const glm::vec2& size)
    {
        left_top = pos;
        right_bottom = pos + size;
    }

    bool hit_test(const glm::vec2& pos)
    {
        return pos.x >= left_top.x && pos.y >= left_top.y &&
                pos.x <= right_bottom.x && pos.y <= right_bottom.y;
    }
};

struct WidgetCallback
{
    using Callback = void(*)(Widget*, void*);
    Callback callback;
    void* user_data;
};

class Widget : public Object
{
public:
    Widget( ObjectManager* manager,
            const glm::vec2& size );

    virtual ~Widget();

    virtual void init(ResourceStorage* storage);

    virtual void draw();

    void remove_children();

    virtual void add_child( Widget* node );

    void remove_child( Widget* node );

    void remove_from_parent( );

    bool is_contains( Widget* child );

    bool get_child_index( Widget* node, basic::uint32 &out_index );

    Widget* get_parent();

    void add_press_callback(WidgetCallback cb);

    void set_position(const glm::vec2& pos);

    void set_size(const glm::vec2& size);

    void set_anchor_point(const glm::vec2& anchor_point);

protected:
    virtual void on_mouse_pressed(input::MouseButton btn , basic::int32 x, basic::int32 y);

    virtual void on_mouse_move(basic::int32 x, basic::int32 y);

    virtual void on_key_pressed(input::KeyCode btn, basic::int16 sym);

    void update_rect();

    glm::mat4 get_matrix() const;

private:
    glm::vec2 m_pos;
    glm::vec2 m_size;
    glm::vec2 m_anchor_point;
    Rect m_rect;
    Widget* m_parent;
    basic::Vector<Widget*> m_children;
    ICamera* m_camera;
    RenderNode* m_view;
    basic::Vector<WidgetCallback> m_press_callbacks;
};
