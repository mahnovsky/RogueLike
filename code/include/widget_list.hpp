#pragma once

#include "widget.hpp"

enum class ListDirection
{
    Horizontal,
    Vertical
};

class WidgetList : public Widget
{
public:
    WidgetList(core::WidgetSystem* root);
    ~WidgetList() override;

    void set_direction(ListDirection dir);

    void add_child( Widget* child ) override;

	void remove_child(Widget* child) override;
private:
    void update_list();

private:
    ListDirection m_direction;
};
