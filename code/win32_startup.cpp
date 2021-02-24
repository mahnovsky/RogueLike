
#include <windows.h>

#include "engine.hpp"
#include "system_manager.hpp"

#include "widget_system.hpp"
#include "resource_storage.hpp"

void game_init(Engine*);

HINSTANCE g_instance;

int __stdcall WinMain( HINSTANCE instance,
                       HINSTANCE prev_instance,
                       LPSTR cmd,
                       int show )
{
    g_instance = instance;
	
	IEngine* engine = NEW_OBJ(Engine, 0, nullptr);

	core::SystemManager& system_manager = engine->get_system_manager();

	system_manager.add_system<core::ResourceStorage>();
	system_manager.add_system<core::WidgetSystem>();

	engine->set_callback(Init, &game_init);
	
	if (engine->init(1024, 768, "RogueLike"))
	{
		while (engine->is_runned() && engine->update());

		engine->cleanup();
	}

	DELETE_OBJ(engine);

	return EXIT_SUCCESS;
}
