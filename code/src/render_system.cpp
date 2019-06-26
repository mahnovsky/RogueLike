#include "render_system.hpp"

#include "render_common.hpp"
#include "static_mesh.hpp"

#include "camera.hpp"
#include "material.hpp"

RenderSystem::RenderSystem( EntityComponentSystem* ecs )
    : m_camera( nullptr )
    , m_transform_id( 0 )
    , m_render_id( 0 )
{
}

RenderSystem::~RenderSystem( )
{
}

void
RenderSystem::initialize( EntityComponentSystem* ecs, ICamera* cam )
{
    ecs->registry_component< RenderComponent >( "RenderComponent" );
    ecs->registry_component< TransformComponent >( "TransformComponent" );

    m_transform_id = ecs->get_component_id< TransformComponent >( );
    m_render_id = ecs->get_component_id< RenderComponent >( );

    ecs->subscribe( m_transform_id, this );
    ecs->subscribe( m_render_id, this );

    m_camera = cam;
}

void
RenderSystem::draw( EntityComponentSystem* ecs )
{
    ASSERT( m_camera );

    const basic::Vector< RenderComponent* >& components = ecs->get_components< RenderComponent >( );

    for ( auto comp : components )
    {
        glm::mat4 model{1.f};
        if ( comp->transform )
        {
            model = comp->transform->tr.get_matrix( );
        }

        draw( comp, model );
    }
}

void
RenderSystem::draw( RenderComponent* component, const glm::mat4& model )
{
    ASSERT( component );

    glBindVertexArray( component->array_object );

    component->material.enable( );

    glm::mat4 mvp = model;
    if ( m_camera )
    {
        glm::mat4 pv( 1.f );
        m_camera->get_matrix( pv );

        mvp = pv * mvp;
    }

    component->material.set_uniform( "MVP", mvp );
    component->material.set_uniform( "Color", component->color );

    if ( component->index_elements > 0 )
    {
        glDrawElements( GL_TRIANGLES, component->index_elements, GL_UNSIGNED_SHORT, nullptr );
    }
    else if ( component->vertex_elements > 0 )
    {
        glDrawArrays( GL_TRIANGLES, 0, component->vertex_elements );
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

            if ( rc && tc )
            {
                rc->transform = tc;
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
            rc->transform = nullptr;
            if ( rc->mesh )
            {
                rc->mesh->release( );
            }
        }
    }
    break;
    default:
        break;
    }
}

void
RenderSystem::load_component( RenderComponent* comp, StaticMesh* m )
{
    ASSERT( m );

    comp->color = basic::Color{255, 255, 255, 255};

    glGenVertexArrays( 1, &comp->array_object );
    glBindVertexArray( comp->array_object );

    comp->mesh = m;
    m->retain( );

    m->bind( );

    m->apply_fmt( );

    comp->index_elements = static_cast< basic::int32 >( m->get_index_count( ) );
    comp->vertex_elements = static_cast< basic::int32 >( m->get_vertex_count( ) );

    glBindVertexArray( 0 );
}
