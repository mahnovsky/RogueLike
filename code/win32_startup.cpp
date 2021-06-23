#include "engine.hpp"
#include "core.hpp"

#include <windows.h>
#include "path.hpp"

void game_init(core::Path);
int game_loop();

HINSTANCE g_instance;

int __stdcall WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_  HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
    _In_ int nCmdShow )
{
    g_instance = hInstance;
	char exeDirectory[1024]; //to store the directory
	DWORD ret = GetModuleFileName(NULL, exeDirectory, 1024);
	core::Path root;
	if (ret)
	{
		root = core::Path::parse(std::string(exeDirectory, exeDirectory + ret));
	}
	
	game_init(root);

	int exit_code = game_loop();
	if (exit_code < 0)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
