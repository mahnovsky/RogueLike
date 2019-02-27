#include "widget.hpp"

#include "render_common.hpp"
#include "object_manager.hpp"
#include "shader.hpp"
#include "camera.hpp"

Widget::Widget(ObjectManager* manager, const glm::vec2& size)
    : Object ( manager )
    , m_pos()
    , m_size(size)
    , m_rect()
    , m_parent(nullptr)
    , m_children()
    , m_debug_rect( nullptr )
{
    m_rect.update(m_pos, size);
}

Widget::~Widget()
{
    remove_node( m_debug_rect );
    m_debug_rect = nullptr;

    for(basic::uint32 i = 0; i < m_children.get_size(); ++i)
    {
        m_children[i]->release();
    }
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
    if( !get_child_index( node, index ) )
    {
        node->retain();

        node->remove_from_parent();
        node->m_parent = this;

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
    }
}

void Widget::remove_from_parent()
{
    if( m_parent )
    {
        m_parent->remove_child( this );
    }
    m_parent = nullptr;
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

void Widget::add_press_callback(WidgetCallback cb)
{
    m_press_callbacks.push(cb);
}

void Widget::set_position(const glm::vec2 &pos)
{
    m_pos = pos;
    update_debug_rect();
}

void Widget::set_size(const glm::vec2 &size)
{
    m_size = size;
    update_debug_rect();
}

void Widget::on_mouse_pressed( input::MouseButton btn, basic::int32 x, basic::int32 y )
{
    if(m_rect.hit_test({x, y}))
    {
        for(basic::uint32 i = 0; i < m_press_callbacks.get_size(); ++i)
        {
            WidgetCallback& cb = m_press_callbacks[i];
            if( cb.callback )
            {
                cb.callback(this, cb.user_data);
            }
        }
    }

    for(basic::uint32 i = 0; i < m_children.get_size(); ++i)
    {
        m_children[i]->on_mouse_pressed(btn, x, y);
    }
}

void Widget::on_mouse_move(basic::int32 x, basic::int32 y)
{
    for(basic::uint32 i = 0; i < m_children.get_size(); ++i)
    {
        m_children[i]->on_mouse_move(x, y);
    }
}

void Widget::on_key_pressed(input::KeyCode code, basic::int16 sym)
{
    for(basic::uint32 i = 0; i < m_children.get_size(); ++i)
    {
        m_children[i]->on_key_pressed(code, sym);
    }
}

void Widget::update_debug_rect()
{
    m_rect.update(m_pos, m_size);
    if(m_debug_rect)
    {
        VertexBufferP vb;
        fill_rect(m_rect.left_top, m_rect.right_bottom, 2.f, vb);
        update_vertices(m_debug_rect, &vb);
    }
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
