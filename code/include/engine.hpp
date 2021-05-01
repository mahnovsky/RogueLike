#pragma once

#include "defines.hpp"
#include "resource_storage.hpp"
#include "ecs_manager.hpp"
#include "system_manager.hpp"

enum EngineCallbackType 
{
    Init,
    Frame,
    Draw,
    Clean,
    OutOfMemory,
    
    Count
};

using engine_callback = void (*)( class Engine* );


class IEngine 
	: public core::TSystem<IEngine, core::SUID_EngineSystem>
{
public:
	IEngine(core::SystemManager& manager)
		:core::TSystem<IEngine, core::SUID_EngineSystem>(manager)
	{}

    virtual ~IEngine() = default;

	virtual void initialize() override {}

    virtual bool init(int width, int height, const char* wnd_title) = 0;

    virtual bool update() = 0;

    virtual void cleanup() = 0;

    virtual void set_callback( EngineCallbackType type, engine_callback callback ) = 0;

    virtual IRender* get_render() = 0;

    virtual input::Input* get_input() = 0;

    virtual glm::vec2 get_window_size() const = 0;

    virtual double get_frame_time() const = 0;

    virtual basic::uint32 get_fps() const = 0;

	virtual core::SystemManager& get_system_manager() = 0;

    virtual bool is_runned() const = 0;
};

class Engine 
	: public IEngine
{
public:
	Engine() = delete;
	Engine(const Engine&) = delete;
	Engine(Engine&&) noexcept = delete;

    Engine( int argv, char** argc );
    ~Engine() override;

    bool init(int width, int height, const char* wnd_title) override;

    bool update() override;

    void cleanup() override;

    void set_callback( EngineCallbackType type, engine_callback callback ) override;

    void shutdown() override;

	void destroy() override {}

    IRender* get_render() override;

    input::Input* get_input() override;

    glm::vec2 get_window_size() const override;

    double get_frame_time() const override;

    bool is_runned() const override;

    uint32_t get_fps() const;

    EntityComponentManager* get_ecs() { return m_ecs; }

	core::SystemManager& get_system_manager() { return m_system_manager; }

private:
    void process_event( );
    
    static void out_of_memory();

private:
    static Engine* _instance;

	core::SystemManager m_system_manager;

    IWindow* m_window;

    IRender* m_render;

    input::Input* m_input;

    bool m_is_runned;

    engine_callback m_callbacks[Count];

    basic::Vector<basic::String> m_cmd_args;

    double m_time;

    double m_delta;

    uint32_t m_fps;

    EntityComponentManager* m_ecs;
};
