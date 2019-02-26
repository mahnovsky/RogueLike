#pragma once

#include "defines.hpp"

#include "object.hpp"

struct Rect
{
    glm::vec2 left_top;
    glm::vec2 right_bottom;

    Rect( const glm::vec2& lt, const glm::vec2& tb )
        :left_top(lt)
        ,right_bottom(tb)
    {}
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

    void add_child( Widget* node );

    void remove_child( Widget* node );

    void remove_from_parent( );

    bool is_contains( Widget* child );

    bool get_child_index( Widget* node, basic::uint32 &out_index );

    Widget* get_parent();

private:
    glm::vec2 m_size;
    Rect m_rect;
    Widget* m_parent;
    basic::Vector<Widget*> m_children;
    ICamera* m_camera;
    RenderNode* m_debug_rect;
};
