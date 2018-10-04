#pragma once

#include "render_common.hpp"

class RenderObject;

class IRender
{
public:
    virtual ~IRender( )
    {
    }

    virtual bool init( int width, int height ) = 0;

    virtual void draw( const RenderObject* graphic ) = 0;

    virtual void draw_begin( ) = 0;

    virtual void draw_end( class IWindow* wnd ) = 0;

    static IRender* create( );
};
