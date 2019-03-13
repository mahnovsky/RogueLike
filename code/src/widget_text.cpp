#include "widget_text.hpp"

#include "render_common.hpp"
#include "resource_storage.hpp"
#include "font.hpp"
#include "transform.hpp"

WidgetText::WidgetText(ObjectManager* manager, const glm::vec2 &size)
    : Widget ( manager, size )
    , m_font_name("arial.ttf")
    , m_font( nullptr )
    , m_text()
    , m_text_render(nullptr)
    , m_text_size()
{}

void WidgetText::init(ResourceStorage *storage)
{
    Widget::init( storage );

    m_font = storage->get_resorce<se::Font>(m_font_name.get_cstr());

    m_text_render = m_font->create_text_node();

    m_text_render->camera = get_camera();

    get_view()->children.push( m_text_render );

    update();
}

void WidgetText::set_text(const basic::String &text)
{
    m_text = text;

    update();
}

void WidgetText::set_align(AlignH horizontal)
{
    m_horizontal = horizontal;

    apply_align();
}

void WidgetText::set_align(AlignV vertical)
{
    m_vertical = vertical;

    apply_align();
}

void WidgetText::update()
{
    if(!m_text.is_empty())
    {
        m_font->update( m_text.get_cstr(), m_text_render, m_text_size );

        apply_align();
    }
}

void WidgetText::apply_align()
{
    glm::vec2 size = get_size();
    float x_align = 0;//-(size.x - m_text_size.x) / 2;
    float y_align = -(size.y - m_text_size.y) / 2 - m_text_size.y;

    switch (m_horizontal)
    {
    case AlignH::Center: x_align = -(size.x - m_text_size.x) / 2; break;
    case AlignH::Left: x_align = 0; break;
    case AlignH::Right: x_align = -(size.x - m_text_size.x); break;
    }

    switch (m_vertical)
    {
    case AlignV::Center: y_align = -(size.y - m_text_size.y) / 2 - m_text_size.y; break;
    case AlignV::Top: y_align = -m_text_size.y; break;
    case AlignV::Bottom: y_align = -(m_text_size.y + (size.y - m_text_size.y)); break;
    }

    glm::vec3 pos{ x_align, y_align, 0.f};

    m_text_render->transform->set_pivot_point(pos);
}
