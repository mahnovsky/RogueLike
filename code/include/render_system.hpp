#pragma once

#include "entity.hpp"
#include "entity_component_system.hpp"
#include "render_common.hpp"
#include "transform.hpp"

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

struct TransformComponent
{
    Transform tr;
};

class TransformSystem : public ISystem
{
public:
    void update( EntityComponentSystem* ecs );

    void on_component_event( Entity* ent,
                             basic::uint32 component_id,
                             ComponentAction act ) override;

private:
};

class RenderSystem : public ISystem
{
public:
    RenderSystem( );
    ~RenderSystem( ) override;

    void initialize( EntityComponentSystem* ecs, ICamera* cam );

    void draw( EntityComponentSystem* ecs );

    void draw( RenderComponent* component );

    void on_component_event( Entity* ent,
                             basic::uint32 component_id,
                             ComponentAction act ) override;

    static void load_component( RenderComponent* comp, const Mesh& m );

private:
    ICamera* m_camera;
    basic::uint32 m_transform_id;
    basic::uint32 m_render_id;
};
