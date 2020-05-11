#pragma once

#include "entity.hpp"
#include "type_registration.hpp"

#include "render_common.hpp"
#include "transform.hpp"

class IRenderObject;
class RenderComponent;

class RenderComponent : public Component
{
public:
	GENERIC_OBJECT_IMPL(RenderComponent, NS_COMPONENT_TYPE);

	RenderComponent(Entity* ent)
		:Component(ent)
		, transform(nullptr)
		, m_render_object(nullptr)
	{}

    glm::mat4 model;

    class Transform* transform;

	basic::String camera;

	void initialize(IRenderObject* obj);

	void update_color() const;

	void update_mvp(const glm::mat4& mvp) const;

	void on_resource_changed(RenderResourceType type, const basic::String& name);

	const basic::String& get_resource_name(RenderResourceType type) const;

	void set_resource_name(RenderResourceType type, const basic::String& name);

	void set_color(basic::Color color);
	
	friend class RenderSystem;

private:
	IRenderObject* m_render_object;

	constexpr static basic::uint32 resources_count = static_cast<basic::uint32>(RenderResourceType::Count);
	basic::String m_resources[resources_count];
	basic::Color color = { 255, 255, 255, 255 };
};

class RenderSystem : public IGenericObject
{
public:
	GENERIC_OBJECT_IMPL(RenderSystem, NS_SYSTEM_TYPE);

	RenderSystem(const RenderSystem&) = delete;
	RenderSystem(RenderSystem&&) noexcept = delete;

    RenderSystem(EcsManager* ecs);
    ~RenderSystem( ) override = default;


	void update(float dt) 
	{
		draw(m_ecs);
	}

    void initialize( IRender* render, ICamera* cam );

    void draw(EcsManager* ecs ) const;

	int get_draw_object_count() const
	{
		return m_draw_object_count;
	}

private:
	EcsManager* m_ecs;
	IRender* m_render;
    ICamera* m_camera;
    basic::uint32 m_transform_id;
    basic::uint32 m_render_id;
	mutable int m_draw_object_count;
};
