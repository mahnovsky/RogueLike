#include "widget_list.hpp"


WidgetList::WidgetList(core::WidgetSystem* root)
    :Widget (root)
    ,m_direction(ListDirection::Vertical)
{
}

WidgetList::~WidgetList()
{
}

void WidgetList::set_direction(ListDirection dir)
{
	m_direction = dir;
}

void WidgetList::add_child(Widget *child)
{
    Widget::add_child( child );

    update_list();
}

void WidgetList::remove_child(Widget * child)
{
	Widget::remove_child(child);

	update_list();
}

void WidgetList::update_list()
{
    glm::vec2 new_pos;

    basic::uint32 children = get_child_count();
    for(basic::uint32 i = 0; i < children; ++i)
    {
        Widget* child = get_child(i);

        child->set_position(new_pos);
        glm::vec2 world_pos = child->get_world_position();
        child->set_visible(hit_test(world_pos));

        glm::vec2 size = child->get_size();

        if( m_direction == ListDirection::Vertical )
        {
            new_pos.y += size.y;
        }
        else
        {
            new_pos.x += size.x;
        }
    }
}
