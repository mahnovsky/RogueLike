#pragma once

#include "defines.hpp"
#include "render_common.hpp"

class ICamera;
class RenderComponent;

class IRenderObject
{
public:
	virtual ~IRenderObject() = default;

	virtual void on_resource_changed(RenderResourceType type, const basic::String& name) = 0;

	virtual void on_component_changed(RenderComponent& comp) = 0;

	virtual void update_mvp(const glm::mat4& mvp) = 0;

	virtual void update_color(basic::Color color) = 0;
};

class IRender
{
public:
    virtual ~IRender( ) = default;

    virtual bool init( ResourceStorage* rs, int width, int height ) = 0;

    virtual void clear( ) = 0;

    virtual void present() = 0;

	virtual IRenderObject* create_object(RenderComponent& comp) = 0;

	virtual void delete_object(IRenderObject* obj) = 0;

    static IRender* create( );
};
