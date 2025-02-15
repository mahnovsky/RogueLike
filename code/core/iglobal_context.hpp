#pragma once

#include "defines.hpp"

namespace core
{
	class IEngine;
	class IGameInstance;
	class SystemManager;

	class IGlobalContext
	{
	public:
		virtual ~IGlobalContext() = default;
		virtual std::string_view get_window_title() const = 0;
		virtual glm::ivec2 get_screen_resolution() const = 0;
		virtual IEngine* get_engine() = 0;
		virtual IGameInstance* get_game_instance() = 0;
		virtual SystemManager* get_system_manager() = 0;


		static void SetGlobalContext(IGlobalContext* context);
		static IGlobalContext* GetInstance();

	private:
		static IGlobalContext* _context;
	};

}