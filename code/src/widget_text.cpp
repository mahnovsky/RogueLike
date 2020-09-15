#include "widget_text.hpp"

#include "render_common.hpp"
#include "resource_storage.hpp"
#include "font.hpp"
#include "transform.hpp"
#include "render_system.hpp"
#include "render.hpp"
#include "root_widget.hpp"
#include "camera.hpp"

WidgetText::WidgetText(RootWidget* root)
    : Widget (root)
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

    m_font = storage->get_resorce<se::Font>(m_font_name.c_str());

    if(m_font)
    {
        update();
    }
}

void WidgetText::set_text(const std::string& text)
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
    if(!m_text.empty() && m_text_render)
    {
		glm::mat4 vp;
		m_root->get_ui_camera()->get_matrix(vp);

		glm::mat4 mvp = vp * glm::translate(glm::mat4{ 1.f }, {400.f, 300.f, 0.f});
		m_text_render->update_mvp(mvp);
		m_text_render->update_color({ 255,255,255,255 });
		m_text_render->set_render_state(RSF_CULL_TEST);
		m_font->update(m_text.c_str(), m_text_render, m_text_size);

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

void WidgetText::draw(IRender* render)
{
	if (!m_text_render)
	{
		m_text_render = render->create_object();
		
		update();
	}
	render->add_to_frame(m_text_render);
}
