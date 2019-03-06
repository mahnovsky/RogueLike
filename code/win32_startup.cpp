
#include <windows.h>
#include "engine.hpp"

void game_init(Engine*);

HINSTANCE g_instance;

int __stdcall WinMain( HINSTANCE instance,
                       HINSTANCE prev_instance,
                       LPSTR cmd,
                       int show )
{
    g_instance = instance;

	IEngine* engine = NEW_OBJ(Engine, 0, nullptr);

	engine->set_callback(Init, &game_init);

	if (engine->init(1024, 768, "RogueLike"))
	{
		while (engine->update());

		engine->cleanup();
	}

	DELETE_OBJ(engine);

	return EXIT_SUCCESS;
}
