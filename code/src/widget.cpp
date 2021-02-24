#include "widget.hpp"

#include "camera.hpp"
#include "render_common.hpp"
#include "widget_system.hpp"
#include "opengl/resource_shader.hpp"
#include "transform.hpp"
#include "generic_object_manager.hpp"

#include "engine.hpp"
#include "render.hpp"

Widget::Widget(core::WidgetSystem* root)
    : m_root(root)
	, m_world_space_rect({ 1.f, 1.f })
    , m_parent( nullptr )
    , m_children( )
	, m_camera(root->get_ui_camera())
    , m_visible( true )
    , m_align( Align::Center )
    , m_storage( nullptr )
	, m_debug_rect(nullptr)
{
	_initialize_debug_rect();
}

Widget::~Widget( )
{
	if (m_debug_rect)
	{
		DELETE_OBJ(m_debug_rect);
	}
	for (Widget* child : m_children)
	{
		DELETE_OBJ(child);
	}
}

void Widget::draw(IRender* render)
{
	if (m_debug_rect)
	{
		m_debug_rect->draw();
	}

	for (auto child : m_children)
	{
		child->draw(render);
	}
}

void Widget::add_child( Widget* node )
{
    ASSERT( node != nullptr );

	if (stdext::push_unique(m_children, node))
	{
		ASSERT(node->m_parent != this);
		if (node->m_parent != nullptr)
		{
			node->remove_from_parent();
		}
		node->m_parent = this;
	}
}

void Widget::remove_child( Widget* node )
{
    ASSERT( node->get_parent( ) == this );

	auto it = std::remove(m_children.begin(), m_children.end(), node);
	m_children.erase(it, m_children.end());

	stdext::remove(m_children, node);
}

void Widget::remove_children( )
{
    m_children.clear( );
}

void Widget::remove_from_parent( )
{
    if ( m_parent )
    {
        m_parent->remove_child( this );
    }
    m_parent = nullptr;
}

bool Widget::is_contains( Widget* child )
{
    ASSERT( child->get_parent( ) == this );

    return stdext::is_contains(m_children, child);
}

bool Widget::get_child_index( Widget* node, uint32_t& out_index ) const
{
    if ( node->get_parent( ) != this )
    {
        return false;
    }

	for (size_t i = 0; i < m_children.size(); ++i)
	{
		if (m_children[i] == node)
		{
			out_index = i;
			return true;
		}
	}
    return false;
}

Widget* Widget::get_parent( )
{
    return m_parent;
}

Widget* Widget::get_child( uint32_t index )
{
    ASSERT( index < m_children.size( ) );

    return m_children.at( index );
}

uint32_t Widget::get_child_count( ) const
{
    return m_children.size( );
}

void Widget::set_press_action( const basic::String& action_name )
{
    m_press_action_name = action_name;
}

void Widget::set_position( const glm::vec2& pos )
{
    m_local_pos = pos;
	m_world_space_rect.pos = pos;

	if (m_parent)
	{
		m_world_space_rect.pos += m_parent->get_world_position();
	}
	if (m_debug_rect)
	{
		m_debug_rect->set_position(m_world_space_rect.pos + glm::vec2(0.5f, 0.5f));
	}
}

glm::vec2 Widget::get_size( ) const
{
    return m_world_space_rect.size;
}

void Widget::set_size( const glm::vec2& size )
{
    m_world_space_rect.size = size;

	if (m_debug_rect)
	{
		m_debug_rect->set_size(size - glm::vec2(1.0f, 1.0f));
	}
}

void Widget::set_anchor_point( const glm::vec2& anchor_point )
{
    m_anchor_point = anchor_point;
}

glm::vec2 Widget::convert_to_world_space( const glm::vec2& pos ) const
{
    return pos;
}

glm::vec2 Widget::get_world_position( ) const
{
    return m_world_space_rect.pos;
}

glm::vec2 Widget::get_local_position() const
{
	return m_local_pos;
}

glm::vec2 Widget::get_left_top_world_position( ) const
{
    glm::vec2 pos = get_world_position( );

    return pos;
}

bool Widget::hit_test( const glm::vec2& point )
{
    return m_world_space_rect.hit_test( point );
}

void Widget::set_visible( bool visible )
{
    m_visible = visible;
}

bool Widget::get_visible( ) const
{
    return m_visible;
}

Align Widget::get_align( ) const
{
    return m_align;
}

void Widget::set_align(Align align)
{
	m_align = align;
}

void Widget::set_picture( Texture* tex )
{

}

void Widget::on_mouse_pressed( input::MouseButton btn, basic::int32 x, basic::int32 y )
{
    if ( !m_press_action_name.is_empty( ) && m_world_space_rect.hit_test( {x, y} ) )
    {
		core::WidgetSystem* root = get_widget_system();
        if ( root )
        {
        }
    }

    for ( basic::uint32 i = 0; i < m_children.size( ); ++i )
    {
        m_children[ i ]->on_mouse_pressed( btn, x, y );
    }
}

void Widget::on_mouse_move( basic::int32 x, basic::int32 y )
{
    for ( basic::uint32 i = 0; i < m_children.size( ); ++i )
    {
        m_children[ i ]->on_mouse_move( x, y );
    }
}

void Widget::on_key_pressed( input::KeyCode code, basic::int16 sym )
{
    for ( basic::uint32 i = 0; i < m_children.size( ); ++i )
    {
        m_children[ i ]->on_key_pressed( code, sym );
    }
}

const ICamera* Widget::get_camera( ) const 
{
    return m_camera;
}

core::WidgetSystem* Widget::get_widget_system( )
{
	return m_root;
}

void Widget::_initialize_debug_rect()
{
	IEngine* engine = m_root->get_engine();
	IRender* render = engine->get_render();
	if ( render )
	{
		m_debug_rect = NEW_OBJ(DrawingRect, render);
		m_debug_rect->set_position(m_world_space_rect.pos);
		m_debug_rect->set_size(m_world_space_rect.size);
		glm::mat4 vp;
		m_root->get_ui_camera()->get_matrix(vp);
		m_debug_rect->set_view_projection_matrix(vp);
		m_debug_rect->set_camera_index(m_root->get_ui_camera()->get_camera_index());
	}
}
