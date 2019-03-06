#include "widget.hpp"

#include "render_common.hpp"
#include "object_manager.hpp"
#include "shader.hpp"
#include "camera.hpp"
#include "transform.hpp"

Widget::Widget(ObjectManager* manager, const glm::vec2& size)
    : Object ( manager )
    , m_pos()
    , m_size(size)
    , m_rect()
    , m_parent(nullptr)
    , m_children()
    , m_view( nullptr )
    , m_visible(true)
{
    m_rect.update({0.f, 0.f}, size);
}

Widget::~Widget()
{
    remove_node( m_view );
    m_view = nullptr;

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
        m_view = make_rect( shader, m_rect.left_top, m_rect.right_bottom, 2.f );
        if(m_view)
        {
            m_view->camera = m_camera;
            m_view->color = {255, 0, 200, 255};
        }
    }
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
        node->update_rect();
    }
}

void Widget::remove_child(Widget *node)
{
	ASSERT(node->get_parent() == this);

	basic::uint32 index;
	if (get_child_index(node, index))
	{
		m_children.remove_by_index(index);

		node->release();
	}
}


void Widget::remove_children()
{
    m_children.clear();
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

bool Widget::get_child_index(Widget *node, basic::uint32 &out_index) const
{
    if( node->get_parent() != this )
    {
        return false;
    }

    return m_children.find_first( out_index, node, 0 );
}

Widget *Widget::get_parent()
{
    return m_parent;
}

Widget *Widget::get_child(basic::uint32 index)
{
    ASSERT( index < m_children.get_size() );
    return m_children.get(index);
}

basic::uint32 Widget::get_child_count() const
{
    return m_children.get_size();
}

void Widget::add_press_callback(WidgetCallback cb)
{
    m_press_callbacks.push(cb);
}

void Widget::set_position(const glm::vec2 &pos)
{
    m_pos = pos;

    update_rect();
}

glm::vec2 Widget::get_size() const
{
    return m_size;
}

void Widget::set_size(const glm::vec2 &size)
{
    m_size = size;

    update_rect();
}

void Widget::set_anchor_point(const glm::vec2 &anchor_point)
{
    m_anchor_point = anchor_point;

    update_rect();
}

glm::vec2 Widget::convert_to_world_space(const glm::vec2 &pos) const
{
    if( m_parent )
    {
        glm::mat4 mat = m_parent->get_matrix();

        return mat * glm::vec4{pos, 0.f, 1.f};
    }

    return pos;
}

glm::vec2 Widget::get_world_position() const
{
    return convert_to_world_space( m_pos );
}

glm::vec2 Widget::get_left_top_world_position() const
{
    glm::vec2 pos = get_world_position();
    if( m_view )
    {
        glm::vec3 pivot = m_view->transform->get_pivot_point();
        pos = glm::vec2{ pos.x - pivot.x, pos.y - pivot.y };
    }

    return pos;
}

bool Widget::hit_test(const glm::vec2 &point)
{
    return m_rect.hit_test(point);
}

void Widget::set_visible(bool visible)
{
    m_visible = visible;
}

bool Widget::get_visible() const
{
	return m_visible;
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

void Widget::update_rect()
{
    if( m_parent )
    {
        m_rect.update( get_left_top_world_position(), m_size );
    }

    if( m_view )
    {
        m_view->transform->set_pivot_point( { m_anchor_point * m_size, 0.f } );

        m_view->transform->set_position( glm::vec3( m_pos, 0.f ) );
        VertexBufferP vb;
        fill_rect(glm::vec2(), m_size, 2.f, vb);
        update_vertices(m_view, &vb);
    }
}

glm::mat4 Widget::get_matrix() const
{
    glm::mat4 mat(1.f);
    if(m_view)
    {
        mat = m_view->transform->get_matrix();
        return (m_parent ? mat * m_parent->get_matrix() : mat);
    }
    return mat;
}

void Widget::draw()
{
    if( !m_visible )
    {
        return;
    }

    if( m_view )
    {
        glm::mat4 mat(1.f);
        if(m_parent)
        {
            mat = m_parent->get_matrix();
        }
        draw_node( m_view, &mat );
    }

    for(basic::uint32 i = 0; i < m_children.get_size(); ++i)
    {    
        m_children[i]->draw();
    }
}
