#pragma once

#include "render.hpp"

class IRenderObjectManager
{
public:
	virtual ~IRenderObjectManager() = default;

	virtual bool init(IRender* render, ResourceStorage* rs, int width, int height) = 0;

	virtual void add_to_frame(IRenderObject* object) = 0;

	virtual IRenderObject* create_object(RenderComponent& comp) = 0;

	virtual void delete_object(IRenderObject* obj) = 0;
};