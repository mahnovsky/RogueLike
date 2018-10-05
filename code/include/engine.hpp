#pragma once

#include "render.hpp"
#include "window.hpp"

class IDrawable
{
public:
    virtual void init( ) = 0;

    virtual void draw( IRender* render ) = 0;
};

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

    int run( int width, int height );

    IRender* get_render();

private:
    void process_event( );
    
    static void out_of_memory();

private:
    static Engine* _instance;

    IWindow* m_window;

    IRender* m_render;

    bool m_quit;

    engine_callback m_callbacks[Count];
};
