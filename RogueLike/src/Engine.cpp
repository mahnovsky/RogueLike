#include "Engine.h"
#include "common.h"

#include <Windows.h>

extern "C"
{
#define STB_DEFINE
#include <stb/stb.h>

#define STB_SPRINTF_IMPLEMENTATION
#include <stb/stb_sprintf.h>
}

SDL_GLContext glcontext;

Engine::Engine()
	:_window(nullptr)
	,_fps(0)
	,_quit(false)
{
}

bool Engine::initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());

		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	char buff[256];
	stb_snprintf(buff, 256, "hello %s", "world");
	SDL_Log(buff);

	_window = SDL_CreateWindow(
		"some title",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		800,
		600,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
	);

	glcontext = SDL_GL_CreateContext(_window);

	glClearColor(0, 1, 0, 1);

	return _window;
}

bool Engine::update()
{
	handleEvents();

	float delta = 0.f;
	Uint32 lastTime = SDL_GetTicks();
	int nbFrames = 0;

	Uint32 time = SDL_GetTicks();
	++nbFrames;
	if ((time - lastTime) > 1000)
	{
		_fps = nbFrames;
		nbFrames = 0;
		lastTime += 1000;
	}

	glClear(GL_COLOR_BUFFER_BIT);



	SDL_GL_SwapWindow(_window);

	Uint32 d = SDL_GetTicks() - time;
	delta = static_cast<float>(d) / 1000.f;

	return !_quit;
}

void Engine::cleanup()
{
	SDL_GL_DeleteContext(glcontext);

	SDL_DestroyWindow(_window);

	SDL_Quit();
}

void Engine::handleEvents()
{
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			_quit = true;
		}
		else if (e.type == SDL_MOUSEMOTION ||
			e.type == SDL_MOUSEBUTTONDOWN ||
			e.type == SDL_MOUSEBUTTONUP)
		{

		}
	}
}
