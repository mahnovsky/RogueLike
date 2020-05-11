#include "widget.hpp"

#include "camera.hpp"
#include "object_manager.hpp"
#include "render_common.hpp"
#include "root_widget.hpp"
#include "shader.hpp"
#include "transform.hpp"
#include "generic_object_manager.hpp"

Widget::Widget( GenericObjectManager* manager )
    : m_object_manager(manager)
    , m_mat( 1.f )
    , m_pos( )
    , m_size(  )
    , m_rect( )
    , m_parent( nullptr )
    , m_children( )
    , m_view( nullptr )
    , m_debug_rect( nullptr )
    , m_picture( nullptr )
    , m_visible( true )
    , m_horizontal( AlignH::Center )
    , m_vertical( AlignV::Center )
    , m_storage( nullptr )
{
    m_object_manager->add_object(this);
    set_name("default_widget");
    m_rect.update({ 0.f, 0.f }, { 100.f, 100.f });
}

Widget::~Widget( )
{
    m_object_manager->remove_object(this);
    RenderNode::remove_node( m_view );
    m_view = nullptr;

    for ( basic::uint32 i = 0; i < m_children.get_size( ); ++i )
    {
        //m_children[ i ]->release( );
    }
}

void
Widget::init( ResourceStorage* storage )
{
    m_storage = m_object_manager->find_by_type<ResourceStorage, NS_SYSTEM_TYPE>();

    m_storage = storage;

    if ( !m_camera )
    {
        return;
    }

    const auto shader = storage->get_resorce< ShaderProgram >( "primitive" );

    if ( shader )
    {
        m_view = RenderNode::create_node( shader, nullptr );
        m_view->set_camera( m_camera );

        m_debug_rect = make_rect( shader, m_rect.left_top, m_rect.right_bottom, 2.f );
        m_view->add_child( m_debug_rect );

        m_debug_rect->set_color( {255, 0, 200, 255} );
    }
}

void
Widget::add_child( Widget* node )
{
    ASSERT( node != nullptr );

    basic::uint32 index = 0;
    if ( !get_child_index( node, index ) )
    {
        node->remove_from_parent( );
        node->m_parent = this;

        m_children.push( node );

        m_view->get_transform( )->add_child( node->m_view->get_transform( ) );
        node->update_rect( );
        node->update_mat( );
    }
}

void
Widget::remove_child( Widget* node )
{
    ASSERT( node->get_parent( ) == this );

    basic::uint32 index;
    if ( get_child_index( node, index ) )
    {
        m_children.remove_by_index( index );
    }
}

void
Widget::remove_children( )
{
    m_children.clear( );
}

void
Widget::remove_from_parent( )
{
    if ( m_parent )
    {
        m_parent->remove_child( this );
    }
    m_parent = nullptr;
}

bool
Widget::is_contains( Widget* child )
{
    ASSERT( child->get_parent( ) == this );

    return m_children.is_contains( child );
}

bool
Widget::get_child_index( Widget* node, basic::uint32& out_index ) const
{
    if ( node->get_parent( ) != this )
    {
        return false;
    }

    return m_children.find_first( out_index, node, 0 );
}

Widget*
Widget::get_parent( )
{
    return m_parent;
}

Widget*
Widget::get_child( basic::uint32 index )
{
    ASSERT( index < m_children.get_size( ) );
    return m_children.get( index );
}

basic::uint32
Widget::get_child_count( ) const
{
    return m_children.get_size( );
}

void
Widget::set_press_action( const basic::String& action_name )
{
    m_press_action_name = action_name;
}

void
Widget::set_position( const glm::vec2& pos )
{
    m_pos = pos;

    update_mat( );
    update_rect( );
}

glm::vec2
Widget::get_size( ) const
{
    return m_size;
}

void
Widget::set_size( const glm::vec2& size )
{
    m_size = size;

    update_rect( );
}

void
Widget::set_anchor_point( const glm::vec2& anchor_point )
{
    m_anchor_point = anchor_point;

    update_mat( );
    update_rect( );
}

glm::vec2
Widget::convert_to_world_space( const glm::vec2& pos ) const
{
    if ( m_parent )
    {
        glm::mat4 mat = m_parent->get_matrix( );

        return mat * glm::vec4{pos, 0.f, 1.f};
    }

    return pos;
}

glm::vec2
Widget::get_world_position( ) const
{
    return convert_to_world_space( m_pos );
}

glm::vec2
Widget::get_left_top_world_position( ) const
{
    glm::vec2 pos = get_world_position( );
    if ( m_view )
    {
        glm::vec3 pivot = m_view->get_transform( )->get_pivot_point( );
        pos = glm::vec2{pos.x - pivot.x, pos.y - pivot.y};
    }

    return pos;
}

bool
Widget::hit_test( const glm::vec2& point )
{
    return m_rect.hit_test( point );
}

void
Widget::set_visible( bool visible )
{
    m_visible = visible;
}

bool
Widget::get_visible( ) const
{
    return m_visible;
}

AlignH
Widget::get_horizontal_align( ) const
{
    return m_horizontal;
}

AlignV
Widget::get_vertical_align( ) const
{
    return m_vertical;
}

void
Widget::set_picture( Texture* tex )
{
    ASSERT( tex );
    if ( !m_picture )
    {
        ASSERT( m_storage != nullptr );
        auto shader = m_storage->get_resorce< ShaderProgram >( "texture" );
        m_picture = RenderNode::create_node( shader, tex );

        VertexBufferT vb;
        IndexBuffer ib;
        QuadGenerator qg( {m_size.x, -m_size.y, -1.f}, {0.f, 1.f}, basic::Color{} );
        qg.generate( vb, 0 );
        qg.generate( ib, 0 );

        m_picture->init_node( &vb, &ib, m_view );
    }
}

void
Widget::on_mouse_pressed( input::MouseButton btn, basic::int32 x, basic::int32 y )
{
    if ( !m_press_action_name.is_empty( ) && m_rect.hit_test( {x, y} ) )
    {
        RootWidget* root = get_root( );
        if ( root )
        {
            root->invoke_action( m_press_action_name, this );
        }
    }

    for ( basic::uint32 i = 0; i < m_children.get_size( ); ++i )
    {
        m_children[ i ]->on_mouse_pressed( btn, x, y );
    }
}

void
Widget::on_mouse_move( basic::int32 x, basic::int32 y )
{
    for ( basic::uint32 i = 0; i < m_children.get_size( ); ++i )
    {
        m_children[ i ]->on_mouse_move( x, y );
    }
}

void
Widget::on_key_pressed( input::KeyCode code, basic::int16 sym )
{
    for ( basic::uint32 i = 0; i < m_children.get_size( ); ++i )
    {
        m_children[ i ]->on_key_pressed( code, sym );
    }
}

void
Widget::update_rect( )
{
    if ( m_parent )
    {
        m_rect.update( get_left_top_world_position( ), m_size );
    }

    if ( m_view )
    {
        m_view->get_transform( )->set_pivot_point( {m_anchor_point * m_size, 0.f} );
        m_view->get_transform( )->set_position( glm::vec3( m_pos, 0.f ) );

        if ( m_debug_rect )
        {
            VertexBufferP vb;
            fill_rect( glm::vec2( ), m_size, 2.f, vb );
            m_debug_rect->update_vertices( &vb );
        }
    }
}

glm::mat4
Widget::get_matrix( ) const
{
    return m_view->get_transform( )->get_matrix( );
}

RenderNode*
Widget::get_view( )
{
    return m_view;
}

ICamera*
Widget::get_camera( )
{
    return m_camera;
}

void
Widget::update_mat( )
{
	m_mat = m_view->get_transform()->get_matrix();
	/*
    m_mat = m_view->get_transform( )->get_matrix( );
    if ( m_parent )
    {
        m_mat = m_mat * m_parent->get_matrix( );
    }

    for ( basic::uint32 i = 0; i < m_children.get_size( ); ++i )
    {
        m_children[ i ]->update_mat( );
    }*/
}

RootWidget*
Widget::get_root( )
{
    if ( m_parent )
    {
        return m_parent->get_root( );
    }

    return dynamic_cast< RootWidget* >( this );
}

void
Widget::draw( )
{
    //if ( !m_visible )
    {
        return;
    }

    m_view->draw_node( );

    for ( basic::uint32 i = 0; i < m_children.get_size( ); ++i )
    {
        m_children[ i ]->draw( );
    }
}
