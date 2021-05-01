#include "render_system.hpp"

#include "render_common.hpp"
#include "opengl/static_mesh.hpp"

#include "render.hpp"
#include "camera.hpp"

RenderComponent::RenderComponent(Entity* ent)
	:Component(ent)
	, transform(nullptr)
	, m_render_object(nullptr)
{
}

RenderComponent::~RenderComponent()
{
}

void RenderComponent::initialize(IRenderObject* obj, uint32_t cam_index)
{
	m_render_object = obj;
	obj->update_color(color);
	obj->set_camera_index(cam_index);
	
	for (basic::uint32 i = 0; i < enum2num(RenderResourceType::Count); ++i)
	{
		auto type = static_cast<RenderResourceType>(i);

		const std::string& name = get_resource_name(type);

		if (!name.empty())
		{
			obj->set_resource(type, name);
		}
	}

	add_listen_component<Transform>();
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

void RenderComponent::on_resource_changed(RenderResourceType type, const std::string& name)
{
	if (m_render_object)
		m_render_object->set_resource(type, name);
}

const std::string& RenderComponent::get_resource_name(RenderResourceType type) const
{
	return m_resources[enum2num(type)];
}

void RenderComponent::set_resource_name(RenderResourceType type, const std::string& name)
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

void RenderComponent::on_event(Component* sender, ComponentEvent event_type)
{
	if (sender->type_index() == TypeInfo<Transform, NS_COMPONENT_TYPE>::type_index &&
		event_type == ComponentEvent::Updated)
	{
		Transform* transform = fast_cast<Transform, NS_COMPONENT_TYPE>(sender);

		m_render_object->update_mvp( transform->get_matrix() );
	}
}

RenderSystem::RenderSystem(EntityComponentManager* ecs)
    : m_ecs(ecs)
	, m_render(nullptr)
	, m_camera( nullptr )
	, m_draw_object_count(0)
{
}

void RenderSystem::update(float dt)
{
	draw(m_ecs);
}

void RenderSystem::initialize(IRender* render, ICamera* cam )
{
	m_render = render;
    m_camera = cam;

	m_render->add_camera(cam);
}

void RenderSystem::draw( EntityComponentManager* ecs ) const
{
    ASSERT( m_camera );

	auto octree = ecs->get_system<Octree>();
	const auto& res = m_camera->get_visible_objects(octree);

	if(res.empty())
	{
		return;
	}

	m_draw_object_count = 0;
	for(auto oc : res)
	{
		auto ent = oc->get_entity();

		auto rc = ent->get_component<RenderComponent>();
		if (!rc->m_render_object)
		{
			rc->initialize(m_render->create_object(), m_camera->get_camera_index());
		}

		/*auto tr = ent->get_component<Transform>();
		
		if (tr)
		{
			rc->update_mvp(tr->get_matrix());
		}*/

		m_render->add_to_frame(rc->m_render_object);

		++m_draw_object_count;
	}
}

int RenderSystem::get_draw_object_count() const
{
	return m_draw_object_count;
}
