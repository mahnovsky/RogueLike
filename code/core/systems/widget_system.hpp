#pragma once

#include "input.hpp"
#include "system.hpp"

class IEngine;
class Widget;

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

		void initialize() override;

		void shutdown() override;

		uint32_t get_camera_index() const;

		const ICamera* get_ui_camera() const;

		IEngine* get_engine();

		Widget* get_root_widget();

	private:
		void key_pressed(input::KeyCode code, basic::int16 key) override;

	private:
		IEngine* m_engine;
		ICamera* m_ui_camera;
		Widget* m_root_widget;
		uint32_t m_camera_index;
	};

}