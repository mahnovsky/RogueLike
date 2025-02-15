#pragma once

#include "entity.hpp"
#include "type_registration.hpp"

#include "render_common.hpp"
#include "transform.hpp"

class IRenderObject;
class RenderComponent;

class RenderComponent : public TGenericObject<RenderComponent, Component, "RenderComponent">
{
public:
	RenderComponent(Entity* ent);
	~RenderComponent() override;

	void initialize(IRenderObject* obj, uint32_t cam_index);

	void update_color() const;

	void update_mvp(const glm::mat4& mvp) const;

	void on_resource_changed(RenderResourceType type, const std::string& name);

	const std::string& get_resource_name(RenderResourceType type) const;

	void set_resource_name(RenderResourceType type, const std::string& name);

	void set_color(basic::Color color);

	void on_event(Component* sender, ComponentEvent event_type) override;
	
	friend class RenderSystem;

public:
	glm::mat4 model;

	class Transform* transform;

	std::string camera;

private:
	IRenderObject* m_render_object;

	constexpr static basic::uint32 resources_count = static_cast<basic::uint32>(RenderResourceType::Count);
	std::string m_resources[resources_count];
	basic::Color color = { 255, 255, 255, 255 };
};

class RenderSystem : public TGenericObject<RenderSystem, IGenericObject, "RenderSystem">
{
public:
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem(RenderSystem&&) noexcept = delete;

    RenderSystem(EntityComponentManager* ecs);
    ~RenderSystem( ) override = default;

	void update(float dt);

    void initialize( IRender* render, ICamera* cam );

    void draw(EntityComponentManager* ecs) const;

	int get_draw_object_count() const;

private:
	EntityComponentManager* m_ecs;
	IRender* m_render;
    ICamera* m_camera;
	mutable int m_draw_object_count;
};
