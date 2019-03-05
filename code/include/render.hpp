#pragma once

#include "render_common.hpp"

class ICamera;
class IRenderObject;

class IRender
{
public:
    virtual ~IRender( );

    virtual bool init( int width, int height ) = 0;

    virtual void clear() = 0;

    virtual void present( class IWindow* wnd ) = 0;

    virtual void push_mvp( const glm::mat4& matrix ) = 0;

    virtual void pop_mvp() = 0;

    //virtual void add_object(RenderNode* node) = 0;

    //virtual void remove_node(RenderNode* node) = 0;

    static IRender* create( );
};
