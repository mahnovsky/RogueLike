#include "widget_button.hpp"
#include "engine.hpp"
#include "render.hpp"


WidgetButton::WidgetButton(core::WidgetSystem* widget)
	:Widget(widget)
	,m_press_event()
	,m_view(nullptr)
	,m_texture_name("btn.png")
{
}

void WidgetButton::initialize()
{
	const auto render = core::get_system<Engine>()->get_render();

	m_view = render->create_object();
	build_rect(m_view, { 0.f, 0.f }, m_world_space_rect.size, DrawMode::Fill);
	m_view->update_color({ 255, 255, 255, 255 });
	m_view->set_camera_index(get_camera_index());
	
	m_view->set_resource(RenderResourceType::ShaderProgram, "texture");
	m_view->set_resource(RenderResourceType::Texture, m_texture_name);

	setup_position(m_view, { m_world_space_rect.pos.x, m_world_space_rect.pos.y, 0.f});
}

void WidgetButton::set_press_event_id(const core::EventId& event_id)
{
	m_press_event = event_id;
}

void WidgetButton::on_mouse_pressed(input::MouseButton btn, basic::int32 x, basic::int32 y)
{
	auto event_system = core::get_system<core::EventSystem>();
	if (!event_system) {
		return;
	}

	if (hit_test({x, y})) {
		auto ev = m_event_pool.alloc(m_press_event, ButtonEventType::Pressed);

		event_system->raise_event(ev);

		m_event_pool.free(ev);
	}
}

void WidgetButton::draw(IRender* render)
{
	Widget::draw(render);

	if (m_view) {
		m_view->update_mvp(m_transform.transform);

		render->add_to_frame(m_view);
	}
}
