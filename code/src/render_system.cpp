#include "render_system.hpp"

#include "render_common.hpp"
#include "static_mesh.hpp"

#include "render.hpp"
#include "camera.hpp"

void RenderComponent::initialize(IRenderObject* obj)
{
	m_render_object = obj;
	obj->update_color(color);
	obj->on_component_changed(*this);
}

void RenderComponent::update_color() const
{
	if(m_render_object)
		m_render_object->update_color(color);
}

void RenderComponent::update_mvp(const glm::mat4& mvp) const
{
	if(m_render_object)
		m_render_object->update_mvp(mvp);
}

void RenderComponent::on_resource_changed(RenderResourceType type, const basic::String& name)
{
	if(m_render_object)
		m_render_object->on_component_changed(*this);
}

const basic::String& RenderComponent::get_resource_name(RenderResourceType type) const
{
	return m_resources[enum2num(type)];
}

void RenderComponent::set_resource_name(RenderResourceType type, const basic::String& name)
{
	const basic::uint32 index = enum2num(type);
	m_resources[index] = name;
	on_resource_changed(type, m_resources[index]);
}

void RenderComponent::set_color(basic::Color color)
{
	this->color = color;
	update_color();
}

RenderSystem::RenderSystem(EcsManager* ecs)
    : m_ecs(ecs)
	, m_render(nullptr)
	, m_camera( nullptr )
    , m_transform_id( 0 )
    , m_render_id( 0 )
{
}

void RenderSystem::initialize(IRender* render, ICamera* cam )
{
	m_render = render;
    m_camera = cam;
}

void RenderSystem::draw( EcsManager* ecs ) const
{
    ASSERT( m_camera );

	auto octree = ecs->get_system<Octree>();
	const auto& res = m_camera->get_visible_objects(octree);

	if(res.empty())
	{
		return;
	}

	glm::mat4 pv(1.f);

	m_camera->get_matrix(pv);

	m_draw_object_count = 0;
	for(auto oc : res)
	{
		auto ent = oc->get_entity();

		auto rc = ent->get_component<RenderComponent>();
		if (!rc->m_render_object)
		{
			rc->initialize(m_render->create_object(*rc));
		}

		auto tr = ent->get_component<Transform>();
		
		if (tr)
		{
			rc->update_mvp(pv * tr->get_matrix());

			tr->is_changed = false;
		}
		else
		{
			rc->update_mvp(pv);
		}

		m_render->add_to_frame(rc->m_render_object);

		++m_draw_object_count;
	}
}
