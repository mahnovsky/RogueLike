#pragma once

#include "ecs/entity_component_system.hpp"

class RenderSystem
{
public:
    RenderSystem( ICamera* camera );

    void draw(EntityComponentSystem* ecs);

    void draw(RenderComponent* component);

private:
    ICamera* m_camera;
};
