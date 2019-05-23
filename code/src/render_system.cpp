#include "render_system.hpp"

#include "render_common.hpp"

#include "camera.hpp"
#include "material.hpp"

RenderSystem::RenderSystem( ICamera* camera )
    : m_camera( camera )
{
}

void
RenderSystem::draw( EntityComponentSystem* ecs )
{
    basic::Vector< RenderComponent* > components = ecs->get_components< RenderComponent >( );

    for ( auto comp : components )
    {
        draw( comp );
    }

    glBindVertexArray( 0 );
}

void
RenderSystem::draw( RenderComponent* component )
{
    glm::mat4 pv( 1.f );
    m_camera->get_matrix( pv );

    component->apply_material( pv );

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
