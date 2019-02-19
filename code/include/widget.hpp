#pragma once

#include "basic/vector.hpp"

#include "object.hpp"
#include "transform.hpp"

class Widget : public Object
{
public:
    Widget();
    virtual ~Widget();

    void remove_children();

    void add_child( Widget* node );

    void remove_child( Widget* node );

    void remove_from_parent( );

    bool is_contains( Widget* child );

    bool get_child_index( Widget* node, basic::uint32 &out_index );

    Widget* get_parent();

public:
    Transform transform;

private:
    Widget* m_parent;
    basic::Vector<Widget*> m_children;
};
