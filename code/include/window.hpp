#pragma once

class IWindow
{
public:
    virtual ~IWindow( )
    {
    }

    virtual bool init( int width, int height, const char* const title ) = 0;

    virtual void process_events( ) = 0;

    virtual void swap_buffers( ) = 0;

    virtual bool is_quit( ) const = 0;

    virtual void get_size( int& out_width, int& out_height ) = 0;

    static IWindow* create( );
};
