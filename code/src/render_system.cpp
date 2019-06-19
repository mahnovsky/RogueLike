#include "render_system.hpp"

#include "render_common.hpp"

#include "camera.hpp"
#include "material.hpp"

RenderSystem::RenderSystem( EntityComponentSystem* ecs )
    : m_camera( nullptr )
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
        draw( comp );
    }
}

void
RenderSystem::draw( RenderComponent* component )
{
    ASSERT( component );
    ASSERT( component->material );

    glBindVertexArray( component->array_object );

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

    if ( component->index_object > 0 )
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
    case ComponentAction::Updated:
    {
        if ( component_id == m_transform_id )
        {
            const TransformComponent* tc = ent->get_component< TransformComponent >( );

            RenderComponent* rc = ent->get_component< RenderComponent >( );
            if ( rc )
            {
                rc->model = tc->tr.get_matrix( );
            }
        }
    }
    break;
    case ComponentAction::Detached:
    {
        if ( component_id == m_render_id )
        {
            RenderComponent* rc = ent->get_component< RenderComponent >( );
            DELETE_OBJ( rc->material );
            rc->material = nullptr;
        }
    }
    break;
    default:
        break;
    }
}

void
RenderSystem::load_component( RenderComponent* comp, const Mesh& m )
{
    comp->color = basic::Color{255, 255, 255, 255};

    glGenVertexArrays( 1, &comp->array_object );

    glBindVertexArray( comp->array_object );

    comp->vertex_object = create_buffer(
            GL_ARRAY_BUFFER, GL_STATIC_DRAW, m.vb.get_raw( ), size( m.vb ) * sizeof( Vertex ) );

    comp->vertex_elements = size_as< basic::int32 >( m.vb );

    if ( !m.ib.is_empty( ) )
    {
        comp->index_object = create_buffer( GL_ELEMENT_ARRAY_BUFFER,
                                            GL_STATIC_DRAW,
                                            m.ib.get_raw( ),
                                            m.ib.get_size( ) * sizeof( basic::uint16 ) );

        comp->index_elements = size_as< basic::int32 >( m.ib );
    }

    basic::Vector< VertexFMT > fmt_list = get_fmt_list( m.vb.get_raw( ) );

    for ( basic::uint32 i = 0; i < fmt_list.get_size( ); ++i )
    {
        const VertexFMT& fmt = fmt_list[ i ];
        glVertexAttribPointer( i,
                               static_cast< GLint >( fmt.size ),
                               fmt.type,
                               fmt.is_normalized,
                               sizeof( Vertex ),
                               reinterpret_cast< const void* >( fmt.offset ) );

        glEnableVertexAttribArray( i );
    }

    glBindVertexArray( 0 );
}
