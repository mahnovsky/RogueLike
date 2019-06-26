#pragma once

#include "entity.hpp"
#include "entity_component_system.hpp"
#include "material.hpp"
#include "render_common.hpp"
#include "transform.hpp"

struct RenderComponent : public IComponent
{
    DECLARE_COMPONENT( RenderComponent )

    basic::uint32 array_object = 0;
    basic::int32 vertex_elements = 0;
    basic::int32 index_elements = 0;
    basic::uint32 flags = 0;
    Material material;
    basic::Color color;
    glm::mat4 model;
    StaticMesh* mesh;
    class TransformComponent* transform;
};

struct TransformComponent : public IComponent
{
    DECLARE_COMPONENT( TransformComponent )

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
    RenderSystem( EntityComponentSystem* ecs );
    ~RenderSystem( ) override;

    void initialize( EntityComponentSystem* ecs, ICamera* cam );

    void draw( EntityComponentSystem* ecs );

    void draw( RenderComponent* component, const glm::mat4& model );

    void on_component_event( Entity* ent,
                             basic::uint32 component_id,
                             ComponentAction act ) override;

    static void load_component( RenderComponent* comp, StaticMesh* m );

private:
    ICamera* m_camera;
    basic::uint32 m_transform_id;
    basic::uint32 m_render_id;
};
