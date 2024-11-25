#include "path.hpp"
#include "game/game_application.hpp"
#include "game/game_instance.hpp"
#include <windows.h>

HINSTANCE g_instance;


void game_init(core::Path root);
int game_loop();

int __stdcall WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_  HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
    _In_ int nCmdShow )
{
	g_instance = hInstance;

	char exeDirectory[1024];
	DWORD ret = GetModuleFileName(NULL, exeDirectory, 1024);
	core::Path root;
	if (ret)
	{
		root = core::Path::parse(std::string(exeDirectory, exeDirectory + ret));
	}
	
	GameApplication app(root);

	app.initialize({ 1024, 768 }, "Cossacs");
	
	while (app.is_running())
	{
		app.update();
	}

	app.cleanup();

	return EXIT_SUCCESS;
}
