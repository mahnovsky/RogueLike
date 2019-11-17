#pragma once

#include "entity.hpp"
#include "entity_component_system.hpp"
#include "material.hpp"
#include "render_common.hpp"
#include "transform.hpp"

class IRenderObject;
class RenderComponent;

struct RenderComponent : public IComponent
{
    DECLARE_COMPONENT( RenderComponent )

    basic::Color color;
    glm::mat4 model;
    class TransformComponent* transform;

	void initialize(IRenderObject* obj);

	void update_mvp(const glm::mat4& mvp);

	void on_resource_changed(RenderResourceType type, const basic::String& name);

	const basic::String& get_resource_name(RenderResourceType type) const;

	void set_resource_name(RenderResourceType type, const basic::String& name);

	friend class RenderSystem;

private:
	IRenderObject* m_render_object;

	constexpr static basic::uint32 resources_count = static_cast<basic::uint32>(RenderResourceType::Count);
	basic::String m_resourses[resources_count];
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

    void initialize( IRender* render, EntityComponentSystem* ecs, ICamera* cam );

    void draw( EntityComponentSystem* ecs );

    void draw( RenderComponent* component, const glm::mat4& model );

    void on_component_event( Entity* ent,
                             basic::uint32 component_id,
                             ComponentAction act ) override;

    static void load_component( RenderComponent* comp, StaticMesh* m );

private:
	IRender* m_render;
    ICamera* m_camera;
    basic::uint32 m_transform_id;
    basic::uint32 m_render_id;
};
