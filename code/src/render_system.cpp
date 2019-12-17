#include "render_system.hpp"

#include "render_common.hpp"
#include "static_mesh.hpp"

#include "render.hpp"
#include "camera.hpp"
#include "material.hpp"

void RenderComponent::initialize(IRenderObject* obj)
{
	m_render_object = obj;
	obj->on_component_changed(*this);
}

void RenderComponent::update_color()
{
	m_render_object->update_color(color);
}

inline void RenderComponent::update_mvp(const glm::mat4& mvp)
{
	m_render_object->update_mvp(mvp);
}

void RenderComponent::on_resource_changed(RenderResourceType type, const basic::String& name)
{
	m_render_object->on_component_changed(*this);
}

const basic::String& RenderComponent::get_resource_name(RenderResourceType type) const
{
	return m_resourses[enum2num(type)];
}

void RenderComponent::set_resource_name(RenderResourceType type, const basic::String& name)
{
	const basic::uint32 index = enum2num(type);
	m_resourses[index] = name;
	on_resource_changed(type, m_resourses[index]);
}

void RenderComponent::set_color(basic::Color color)
{
	this->color = color;
	update_color();
}

RenderSystem::RenderSystem( EntityComponentSystem* ecs )
    : m_render(nullptr)
	, m_camera( nullptr )
    , m_transform_id( 0 )
    , m_render_id( 0 )
{
}

RenderSystem::~RenderSystem( )
{
}

void
RenderSystem::initialize(IRender* render, EntityComponentSystem* ecs, ICamera* cam )
{
    ecs->registry_component< RenderComponent >( "RenderComponent" );
    ecs->registry_component< TransformComponent >( "TransformComponent" );

    m_transform_id = ecs->get_component_id< TransformComponent >( );
    m_render_id = ecs->get_component_id< RenderComponent >( );

    ecs->subscribe( m_transform_id, this );
    ecs->subscribe( m_render_id, this );

	m_render = render;
    m_camera = cam;
}

void
RenderSystem::draw( EntityComponentSystem* ecs )
{
    ASSERT( m_camera );

    const basic::Vector< RenderComponent* >& components = ecs->get_components< RenderComponent >( );

    for ( auto comp : components )
    {
        if ( comp->transform && comp->transform->tr.is_changed )
        {
			glm::mat4 pv(1.f);
			m_camera->get_matrix(pv);

            comp->update_mvp( pv * comp->transform->tr.get_matrix( ) );

			comp->transform->tr.is_changed = false;
        }
    }
}

void
RenderSystem::on_component_event( Entity* ent, basic::uint32 component_id, ComponentAction act )
{
    switch ( act )
    {
    case ComponentAction::Attached:
    {
        if ( component_id == m_transform_id || component_id == m_render_id )
        {
            TransformComponent* tc = ent->get_component< TransformComponent >( );
            RenderComponent* rc = ent->get_component< RenderComponent >( );

			if (rc)
			{
				rc->initialize( m_render->create_object(*rc) );
				
				if (tc)
				{
					rc->transform = tc;
				}
			}
        }
    }
    break;
    case ComponentAction::Updated:
        break;
    case ComponentAction::Detached:
    {
        if ( component_id == m_transform_id )
        {
            RenderComponent* rc = ent->get_component< RenderComponent >( );
			m_render->delete_object(rc->m_render_object);
            rc->transform = nullptr;
        }
    }
    break;
    default:
        break;
    }
}