#include "widget.hpp"

#include "render_common.hpp"
#include "object_manager.hpp"
#include "shader.hpp"
#include "camera.hpp"

Widget::Widget(ObjectManager* manager, const glm::vec2& size)
    : Object ( manager )
    , m_size(size)
    , m_rect({0, size.y}, {size.x, 0})
    , m_parent(nullptr)
    , m_children()
    , m_debug_rect( nullptr )
{
}

Widget::~Widget()
{
    remove_node( m_debug_rect );

    remove_from_parent();
}

void Widget::init( ResourceStorage *storage )
{
    m_camera = dynamic_cast<ICamera*>( get_manager()->find( "ui_camera" ) );
    if( !m_camera )
    {
        return;
    }

    ShaderProgram* shader = storage->get_resorce<ShaderProgram>("primitive");
    if(shader)
    {
        m_debug_rect = make_rect( shader, m_rect.left_top, m_rect.right_bottom, 2.f );
        if(m_debug_rect)
        {
            m_debug_rect->camera = m_camera;
            m_debug_rect->color = {255, 0, 200, 255};
        }
    }
}

void Widget::remove_children()
{
    m_children.clear();
}

void Widget::add_child(Widget *node)
{
    ASSERT( node != nullptr );

    basic::uint32 index = 0;
    if( get_child_index( node, index ) )
    {
        node->retain();

        node->remove_from_parent();

        m_children.push( node );
    }
}

void Widget::remove_child(Widget *node)
{
    ASSERT(node->get_parent() == this);

    basic::uint32 index;
    if( get_child_index( node, index ) )
    {
        m_children.remove_by_index( index );

        node->release();

        return;
    }

    ASSERT_M( false, "Child not found something wrong" );
}

void Widget::remove_from_parent()
{
    if( m_parent )
    {
        m_parent->remove_child( this );
    }
}

bool Widget::is_contains(Widget *child)
{
    ASSERT( child->get_parent() == this );

    return m_children.is_contains( child );
}

bool Widget::get_child_index(Widget *node, basic::uint32 &out_index)
{
    if( node->get_parent() == this )
    {
        return m_children.find_first( out_index, node, 0 );
    }

    return false;
}

Widget *Widget::get_parent()
{
    return m_parent;
}

void Widget::draw()
{
    if( m_debug_rect )
    {
        draw_node( m_debug_rect );
    }

    for(basic::uint32 i = 0; i < m_children.get_size(); ++i)
    {
        m_children[i]->draw();
    }
}
