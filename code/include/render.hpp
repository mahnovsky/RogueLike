#pragma once

#include "render_common.hpp"

class ICamera;
class IRenderObject;

class IRender
{
public:
    virtual ~IRender( );

    virtual bool init( int width, int height ) = 0;

    virtual void draw( ICamera* camera, const IRenderObject* graphic ) = 0;

    virtual void draw_begin( ) = 0;

    virtual void draw_end( class IWindow* wnd ) = 0;

    virtual void push_mvp( const glm::mat4& matrix ) = 0;

    virtual void pop_mvp() = 0;

    static IRender* create( );
};
