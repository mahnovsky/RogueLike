#include "render_system.hpp"

#include "render_common.hpp"

#include "camera.hpp"
#include "material.hpp"

RenderSystem::RenderSystem( )
    : m_camera( nullptr )
{
}

void
RenderSystem::initialize( EntityComponentSystem* ecs, ICamera* cam )
{
    ecs->registry_component< RenderComponent >( "RenderComponent" );

    m_camera = cam;
}

void
RenderSystem::draw( EntityComponentSystem* ecs )
{
    ASSERT( m_camera );

    basic::Vector< RenderComponent* > components = ecs->get_components< RenderComponent >( );

    for ( auto comp : components )
    {
        draw( comp );
    }
}

void
RenderSystem::draw( RenderComponent* component )
{
    ASSERT( component );
    ASSERT( component->material );

    component->material->enable( );

    glm::mat4 mvp = component->model;
    if ( m_camera )
    {
        glm::mat4 pv( 1.f );
        m_camera->get_matrix( pv );

        mvp = pv * mvp;
    }

    component->material->set_uniform( "MVP", mvp );
    component->material->set_uniform( "Color", component->color );

    glBindVertexArray( component->array_object );

    if ( component->index_object > 0 )
    {
        glDrawElements( GL_TRIANGLES, component->index_elements, GL_UNSIGNED_SHORT, nullptr );
    }
    else if ( component->vertex_elements > 0 )
    {
        glDrawArrays( GL_TRIANGLES, 0, component->vertex_elements );
    }
}
