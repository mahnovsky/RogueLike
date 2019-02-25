#pragma once

#include "basic/vector.hpp"

#include "object.hpp"
#include "transform.hpp"

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
    Widget(const glm::vec2& size);
    virtual ~Widget();

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
};
