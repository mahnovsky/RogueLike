#include "widget_text.hpp"

#include "render_common.hpp"
#include "resource_storage.hpp"
#include "font.hpp"
#include "transform.hpp"

WidgetText::WidgetText(GenericObjectManager* manager)
    : Widget ( manager )
    , m_font_name("arial.ttf")
    , m_font( nullptr )
    , m_text()
    , m_text_render(nullptr)
{}

WidgetText::~WidgetText()
{
}

void WidgetText::init(ResourceStorage *storage)
{
    Widget::init( storage );

    m_font = storage->get_resorce<se::Font>(m_font_name.get_cstr());

    if(m_font)
    {
        update();
    }
}

void WidgetText::set_text(const basic::String &text)
{
    m_text = text;

    update();
}

void WidgetText::set_color(const basic::Color &color)
{
}

basic::Color WidgetText::get_color() const
{
	return {};
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

	update_mat();
}
