#pragma once

#include "input.hpp"
#include "system.hpp"

class IEngine;
class Widget;
class WidgetEvent;
class IWidgetEventListener;

class IRectPointerListener
{
public:
	virtual bool hit_test(int32_t pos_x, int32_t pos_y) const = 0;

	virtual void pointer_enter_event() = 0;
};

namespace core
{

	class WidgetSystem
		: public TSystem< WidgetSystem, SystemUID::SUID_WidgetSystem >
		, public input::InputListener
	{
	public:
		WidgetSystem(SystemManager& manager);
		~WidgetSystem() override;

		void initialize(IGlobalContext* context) override;

		void update() override;

		void draw() override;

		void shutdown() override;

		const ICamera* get_ui_camera() const;

		IEngine* get_engine() const;

		Widget* get_root_widget() const;

	private:
		void key_pressed(input::KeyCode code, basic::int16 key) override;

		void on_mouse_event(const input::MouseEvent& mouse_event) override;

	private:
		IEngine* m_engine;
		ICamera* m_ui_camera;
		Widget* m_root_widget;
	};

}