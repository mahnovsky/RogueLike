#pragma once

#include "render.hpp"
#include "window.hpp"
#include "basic/string.hpp"

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

    IRender* get_render();

    void get_window_size(int& out_width, int& out_height);

    basic::uint64 get_frame_time() const;

private:
    void process_event( );
    
    static void out_of_memory();

private:
    static Engine* _instance;

    IWindow* m_window;

    IRender* m_render;

    bool m_quit;

    engine_callback m_callbacks[Count];

    basic::Vector<basic::String> m_cmd_args;

    basic::uint64 m_delta;
};
