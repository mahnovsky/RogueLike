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

WidgetText::~WidgetText()
{
}

void WidgetText::initialize()
{
	core::SystemManager& manager = m_root->get_system_manager();
	core::ResourceStorage* storage = manager.get_system<core::ResourceStorage>();

	if (!m_font && storage)
	{
		m_font = storage->get_resorce<se::Font>(m_font_name.c_str());

		if (m_font)
		{
			update();
		}
	}
}

void WidgetText::set_text(const std::string& text)
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

void WidgetText::set_align(Align align)
{
    m_align = align;

    apply_align();
}

void WidgetText::update()
{
    if(!m_text.empty() && m_text_render)
    {
		glm::mat4 vp;
		//m_root->get_ui_camera()->get_matrix(vp);

		glm::vec3 pos = glm::vec3(get_local_position(), 0.f);
		glm::mat4 mvp = glm::translate(glm::mat4{ 1.f }, pos);

		m_text_render->update_mvp(mvp);
		m_text_render->update_color(m_color);
		m_text_render->set_render_state(ROF_CULL_TEST);
		m_font->update(m_text.c_str(), m_text.size(), m_text_render, m_text_size);
		set_size(m_text_size);

        apply_align();
    }
}

void WidgetText::apply_align()
{
    glm::vec2 size = get_size();
    float x_align = 0;//-(size.x - m_text_size.x) / 2;
    float y_align = -(size.y - m_text_size.y) / 2 - m_text_size.y;

    switch (m_align)
    {
    case Align::Center: x_align = -(size.x - m_text_size.x) / 2; break;
    case Align::Left: x_align = 0; break;
    case Align::Right: x_align = -(size.x - m_text_size.x); break;
    //case Align::Center: y_align = -(size.y - m_text_size.y) / 2 - m_text_size.y; break;
    case Align::Top: y_align = -m_text_size.y; break;
    case Align::Bottom: y_align = -(m_text_size.y + (size.y - m_text_size.y)); break;
    }

    glm::vec3 pos{ x_align, y_align, 0.f};
}

void WidgetText::draw(IRender* render)
{
	Widget::draw(render);

	if (!m_text_render)
	{
		m_text_render = render->create_object();
		m_text_render->set_camera_index(m_root->get_camera_index());
		update();
	}
	render->add_to_frame(m_text_render);
}
