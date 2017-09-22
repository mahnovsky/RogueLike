#pragma once


class Engine
{
public:
	Engine();

	bool initialize();

	bool update();

	void cleanup();

private:

	void handleEvents();

private:
	struct SDL_Window* _window;

	int _fps;

	bool _quit;
};