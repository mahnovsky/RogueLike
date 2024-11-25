#pragma once

#include "iglobal_context.hpp"
#include "core.hpp"

class GameApplication
	:public core::IGlobalContext
{
public:
	GameApplication(const core::Path& root);

	void initialize(glm::ivec2 screen_size, const char* title);

	void update();

	void cleanup();

	bool is_running() const;

	glm::ivec2 get_screen_resolution() const override;
	IEngine* get_engine() override;
	core::IGameInstance* get_game_instance() override;
	core::SystemManager* get_system_manager() override;

private:
	core::Path _root_dir;
	glm::ivec2 _screen_size;
	IEngine* _engine;
	core::IGameInstance* _game;
	core::SystemManager _systems;
	
	bool _is_running = true;
};

