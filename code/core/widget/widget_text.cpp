#include "widget_text.hpp"

#include "render_common.hpp"
#include "resource_storage.hpp"
#include "font.hpp"
#include "transform.hpp"
#include "render_system.hpp"
#include "render.hpp"
#include "widget_system.hpp"
#include "camera.hpp"
#include "engine.hpp"

WidgetText::WidgetText(core::WidgetSystem* root)
    : Widget (root)
    , m_font_name("arial.ttf")
    , m_font( nullptr )
    , m_text()
    , m_text_render(nullptr)
	, m_text_size()
	, m_color{ 255, 255, 255, 255 }
{
	ASSERT(root != nullptr);
	initialize();
}

void WidgetText::initialize()
{
	core::SystemManager& manager = m_root->get_system_manager();
	auto storage = manager.get_system<core::ResourceStorage>();

	if (!m_font && storage)
	{
		m_font = storage->get_resource<se::Font>(m_font_name.c_str());

		if (m_font)
		{
			update();
		}
	}
}

void WidgetText::set_text(const std::string_view& text)
{
    m_text = text;

    update();
}

void WidgetText::set_color(const basic::Color &color)
{
	m_color = color;
	if (m_text_render)
	{
		m_text_render->update_color(color);
	}
}

basic::Color WidgetText::get_color() const
{
	return m_color;
}

void WidgetText::update()
{
    if(!m_text.empty() && m_text_render)
    {
		m_text_render->set_camera_index(get_camera_index());
		m_text_render->update_color(m_color);
		m_text_render->set_render_state(ROF_CULL_TEST);
		m_font->update(m_text.c_str(), m_text.size(), m_text_render, m_text_size);

        
		if (const auto parent = get_parent())
		{
			set_size(parent->get_size());
		}
		apply_align();
		update_transform();
		m_text_transform.update(get_transform());
    }
}

void WidgetText::apply_align()
{
    const glm::vec2 size = get_size();
    float x_align = 0;
	float y_align = 0;

    switch (m_horizontal_align)
	{
		case HAlign::Center:
			x_align = (size.x - m_text_size.x) / 2;
			break;
		case HAlign::Left:
			x_align = 0;
			break;
		case HAlign::Right:
			x_align = (size.x - m_text_size.x);
			break;
	}
	switch (m_vertical_align)
	{
		case VAlign::Center:
			y_align = (size.y - m_text_size.y) / 2;
			break;
		case VAlign::Top:
			y_align = size.y - m_text_size.y;
			break;
		case VAlign::Bottom:
			y_align = 0;
			break;
	}

    glm::vec3 pos{ x_align, y_align, 0.f};

	m_text_transform.pos = pos;
}

void WidgetText::draw(IRender* render)
{
	Widget::draw(render);

	if (!m_text_render)
	{
		m_text_render = render->create_object();
		
		update();
	}

	m_text_render->update_mvp(m_text_transform.transform);
	render->add_to_frame(m_text_render);
}
