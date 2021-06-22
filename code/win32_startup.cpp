#include "engine.hpp"
#include "core.hpp"

#include <windows.h>

void game_init();
int game_loop();

HINSTANCE g_instance;

int __stdcall WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_  HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
    _In_ int nCmdShow )
{
    g_instance = hInstance;
	
	game_init();

	int exit_code = game_loop();
	if (exit_code < 0)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
