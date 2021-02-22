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
		, m_camera_index(0)
	{
	}

	WidgetSystem::~WidgetSystem()
	{
		DELETE_OBJ(m_ui_camera);
	}

	void WidgetSystem::initialize()
	{
		m_engine = get_system_manager().get_system<IEngine>();

		m_engine->get_input()->add_listener(this);

		const glm::vec2 size = m_engine->get_window_size();
		m_ui_camera = NEW_OBJ(OrthoCamera, size.x, size.y, 0.f, 100.f);
		glm::vec3 zero;
		m_ui_camera->init(zero, zero, zero);
		m_camera_index = m_engine->get_render()->add_camera(m_ui_camera);

		m_root_widget = NEW_OBJ(Widget, this);
		m_root_widget->set_size(size);
		m_root_widget->set_position({ 0.f, 0.0f });
	}

	void WidgetSystem::shutdown()
	{
		m_engine->get_input()->remove_listener(this);

		DELETE_OBJ(m_root_widget);
	}

	uint32_t WidgetSystem::get_camera_index() const
	{
		return m_camera_index;
	}

	const ICamera* WidgetSystem::get_ui_camera() const
	{
		return m_ui_camera;
	}

	IEngine* WidgetSystem::get_engine()
	{
		return m_engine;
	}

	Widget* WidgetSystem::get_root_widget()
	{
		return m_root_widget;
	}

	void WidgetSystem::key_pressed(KeyCode code, basic::int16 key)
	{

	}
}