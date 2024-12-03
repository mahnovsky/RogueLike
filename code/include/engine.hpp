#pragma once

#include "defines.hpp"
#include "resource_storage.hpp"
#include "ecs_manager.hpp"
#include "system_manager.hpp"
#include "iglobal_context.hpp"

enum EngineCallbackType 
{
    Init,
    Frame,
    Draw,
    Clean,
    OutOfMemory,
    
    Count
};

using engine_callback = void (*)( class IEngine* );

class IEngine 
{
public:
    virtual ~IEngine() = default;

    virtual void set_callback( EngineCallbackType type, engine_callback callback ) = 0;

    virtual IRender* get_render() = 0;

    virtual input::Input* get_input() = 0;

    virtual glm::vec2 get_window_size() const = 0;

    virtual double get_frame_time() const = 0;

    virtual basic::uint32 get_fps() const = 0;

    virtual bool is_runned() const = 0;

    virtual class EntityComponentManager* get_ecs() = 0;
};

class Engine 
	: public core::TSystem<Engine, core::SystemUID::SUID_CoreSystem>
	, public IEngine
{
public:
	Engine() = delete;
	Engine(const Engine&) = delete;
	Engine(Engine&&) noexcept = delete;

    Engine(core::SystemManager& manager);
    ~Engine() override = default;

    void initialize(core::IGlobalContext* context) override;

    void shutdown() override;

    void update() override;

    void set_callback( EngineCallbackType type, engine_callback callback ) override;

    IRender* get_render() override;

    input::Input* get_input() override;

    glm::vec2 get_window_size() const override;

    double get_frame_time() const override;

    bool is_runned() const override;

    uint32_t get_fps() const override;

    EntityComponentManager* get_ecs() override { return m_ecs; }

private:
    void process_event( );
    
    static void out_of_memory();

    void update_fps();

private:
    static Engine* _instance;

    core::IGlobalContext* m_context;

    IWindow* m_window;

    IRender* m_render;

    input::Input* m_input;

    bool m_is_runned;

    engine_callback m_callbacks[Count];

    std::vector<std::string> m_cmd_args;

    double m_fps_time;

    double m_frame_time;

    uint32_t m_fps;

    EntityComponentManager* m_ecs;
};
