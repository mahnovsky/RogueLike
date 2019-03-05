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
    WidgetList(ObjectManager* manager, const glm::vec2& size);
    ~WidgetList() override;

    void set_direction(ListDirection dir);

public:
    ListDirection m_direction;
};
