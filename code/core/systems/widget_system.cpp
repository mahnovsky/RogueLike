#include "widget_system.hpp"

#include "engine.hpp"
#include "camera.hpp"
#include "widget.hpp"
#include "render.hpp"

using namespace input;

namespace core
{
	WidgetSystem::WidgetSystem(SystemManager& manager)
		: TSystem< WidgetSystem, SystemUID::SUID_WidgetSystem>(manager)
		, m_engine(nullptr)
		, m_ui_camera(nullptr)
		, m_root_widget(nullptr)
	{
	}

	WidgetSystem::~WidgetSystem()
	{
		DELETE_OBJ(m_ui_camera);
	}

	void WidgetSystem::initialize(IGlobalContext* context)
	{
		m_engine = core::get_system<Engine>();
		m_engine->get_input()->add_listener(this);

		const glm::vec2 size = m_engine->get_window_size();
		m_ui_camera = NEW_OBJ(OrthoCamera, size.x, size.y, 0.f, 100.f);
		const glm::vec3 zero;
		m_ui_camera->init(zero, zero, zero);
		m_engine->get_render()->add_camera(m_ui_camera);

		m_root_widget = NEW_OBJ(Widget, this);
		m_root_widget->set_size(size);
		m_root_widget->set_position({ 0.f, 0.0f });
	}

	void WidgetSystem::update()
	{
		if (m_root_widget) {
			m_root_widget->update();
		}
	}

	void WidgetSystem::draw()
	{
		if (m_root_widget) {
			m_root_widget->draw(m_engine->get_render());
		}
	}

	void WidgetSystem::shutdown()
	{
		m_engine->get_input()->remove_listener(this);

		DELETE_OBJ(m_root_widget);
	}

	const ICamera* WidgetSystem::get_ui_camera() const
	{
		return m_ui_camera;
	}

	IEngine* WidgetSystem::get_engine() const
	{
		return m_engine;
	}

	Widget* WidgetSystem::get_root_widget() const
	{
		return m_root_widget;
	}

	void WidgetSystem::key_pressed(KeyCode code, basic::int16 key)
	{
		m_root_widget->on_key_pressed(code, key);
	}

	void WidgetSystem::on_mouse_event(const input::MouseEvent& mouse_event)
	{
		const auto size = get_engine()->get_window_size();
		const auto pos_y = static_cast<int>(size.y) - mouse_event.pos_y;
		if (mouse_event.type == Pressed) {
			m_root_widget->on_mouse_pressed(mouse_event.button, mouse_event.pos_x, pos_y);
		}
		else if (mouse_event.type == Moved) {
			m_root_widget->on_mouse_move(mouse_event.pos_x, pos_y);
		}
	}
}