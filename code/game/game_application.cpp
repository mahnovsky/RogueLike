#include "game_application.hpp"

#include "transform.hpp"
#include "engine.hpp"

void game_init(core::Path root);
int game_loop();

GameApplication::GameApplication(const core::Path& root)
	:_root_dir(std::move(root))
	,_engine(nullptr)
	,_game(nullptr)
	,_systems(this)
{
	SetGlobalContext(this);
}

void GameApplication::initialize(glm::ivec2 screen_size, const char* title)
{
	Transform* tr = basic::Memory::New<Transform>(nullptr).Get();
	LOG("RTTI test %s, %ull", tr->get_typename(), Transform::GetTypeHash());

	_engine = basic::Memory::New<Engine>(this).Get();

	_systems.add_system<core::FileSystem>();
	_systems.add_system<core::ResourceStorage>();
	_systems.add_system<core::WidgetSystem>();

	_screen_size = screen_size;

	if (_engine->initialize(_screen_size.x, _screen_size.y, title))
	{
		game_init(_root_dir);
	}
}

void GameApplication::update()
{
	_systems.update();

	if (!_engine->update())
	{
		_is_running = false;
	}
}

void GameApplication::cleanup()
{
	_systems.shutdown();
	_engine->cleanup();
	basic::Memory::Delete(_engine);

	basic::Memory::PrintMemStats();
}

bool GameApplication::is_running() const 
{
	return _is_running;
}

glm::ivec2 GameApplication::get_screen_resolution() const
{
	return _screen_size;
}

IEngine* GameApplication::get_engine()
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