#pragma once

#include "defines.hpp"

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

class Engine
{
public:
    Engine( int argv, char** argc );

    void set_callback( EngineCallbackType type, engine_callback callback ); 

    int run( int width, int height, const char* wnd_title );

    void quit();

    IRender* get_render();

    input::Input* get_input();

    void get_window_size(int& out_width, int& out_height);

    glm::vec2 get_window_size() const;

    double get_frame_time() const;

    basic::uint32 get_fps() const;

private:
    void process_event( );
    
    static void out_of_memory();

private:
    static Engine* _instance;

    IWindow* m_window;

    IRender* m_render;

    input::Input* m_input;

    bool m_quit;

    engine_callback m_callbacks[Count];

    basic::Vector<basic::String> m_cmd_args;

    double m_time;

    double m_delta;

    basic::uint32 m_fps;
};
