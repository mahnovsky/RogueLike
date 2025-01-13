#include "game_application.hpp"

#include "transform.hpp"
#include "engine.hpp"
#include "event_system.hpp"

GameApplication::GameApplication(const core::Path& root, core::IGameInstance* instance)
	:_root_dir(std::move(root))
	,_engine(nullptr)
	,_game(instance)
	,_systems(this)
{
	SetGlobalContext(this);
}

bool GameApplication::initialize(glm::ivec2 screen_size, const char* title)
{
	LOG("Start game application %s", title);

	_screen_size = screen_size;
	_window_title = title;

	_systems.add_system<core::Engine>();
	_systems.add_system<core::EventSystem>();
	_systems.add_system<core::FileSystem>();
	_systems.add_system<core::ResourceStorage>();
	_systems.add_system<core::WidgetSystem>();

	_systems.initialize_systems();

	_engine = _systems.get_system<core::Engine>();
	_game->initialize();

	return true;
}

void GameApplication::update()
{
	_systems.update();

	_is_running = _engine->is_runned();
}

void GameApplication::cleanup()
{
	_systems.shutdown();
	
	basic::Memory::PrintMemStats();
}

bool GameApplication::is_running() const 
{
	return _is_running;
}

std::string_view GameApplication::get_window_title() const
{
	return _window_title;
}

glm::ivec2 GameApplication::get_screen_resolution() const
{
	return _screen_size;
}

core::IEngine* GameApplication::get_engine()
{
	return _engine;
}

core::IGameInstance* GameApplication::get_game_instance()
{
	return _game;
}

core::SystemManager* GameApplication::get_system_manager()
{
	return &_systems;
}