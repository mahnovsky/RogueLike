#pragma once

#include "ecs/entity_component_system.hpp"

struct RenderComponent
{
    basic::uint32 array_object = 0;
    basic::uint32 vertex_object = 0;
    basic::int32 vertex_elements = 0;
    basic::uint32 index_object = 0;
    basic::int32 index_elements = 0;
    basic::uint32 flags = 0;
    basic::int32 order = 0;
    Material* material = nullptr;
    basic::Color color;
    glm::mat4 model;
};

class RenderSystem
{
public:
    RenderSystem( );

    void initialize( EntityComponentSystem* ecs, ICamera* cam );

    void draw( EntityComponentSystem* ecs );

    void draw( RenderComponent* component );

private:
    ICamera* m_camera;
};
