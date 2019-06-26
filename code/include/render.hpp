#pragma once

#include "render_common.hpp"

class ICamera;
class IRenderObject;

class IRender
{
public:
    virtual ~IRender( );

    virtual bool init( int width, int height ) = 0;

    virtual void clear( ) = 0;

    virtual void present( class IWindow* wnd ) = 0;

    virtual IGpuFactory* get_factory( ) = 0;

    static IRender* create( );
};
