#include "widget.hpp"

#include "camera.hpp"
#include "render_common.hpp"
#include "widget_system.hpp"
#include "opengl/resource_shader.hpp"
#include "transform.hpp"

#include "engine.hpp"
#include "render.hpp"

Widget::Widget(core::WidgetSystem* root)
    : m_root(root)
	, m_world_space_rect({ 1.f, 1.f })
    , m_parent( nullptr )
    , m_children( )
    , m_visible( true )
    , m_horizontal_align( HAlign::Center )
	, m_vertical_align(VAlign::Center)
    , m_storage( nullptr )
	, m_debug_rect(nullptr)
{
	_initialize_debug_rect();
}

Widget::~Widget( )
{
	if (m_debug_rect)
	{
		core::IEngine* engine = m_root->get_engine();
		IRender* render = engine->get_render();
		if (render)
		{
			render->delete_object(m_debug_rect);
		}
	}
	for (Widget* child : m_children)
	{
		DELETE_OBJ(child);
	}
}

void Widget::update()
{
	update_transform();

	m_world_space_rect.pos = get_world_position();

	for (const auto child : m_children)
	{
		child->update();
	}
}

void Widget::draw(IRender* render)
{
	if (!m_visible) {
		return;
	}

	if (m_debug_rect)
	{
		m_debug_rect->update_mvp(m_transform.transform);
		render->add_to_frame(m_debug_rect);
	}

	for (const auto child : m_children)
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

	stdext::remove(m_children, node);
}

const core::FTransform* Widget::get_transform() const
{
	return &m_transform;
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

Widget* Widget::get_parent( ) const
{
    return m_parent;
}

Widget* Widget::get_child( uint32_t index ) const
{
    ASSERT( index < m_children.size( ) );

    return m_children[index];
}

uint32_t Widget::get_child_count( ) const
{
    return m_children.size( );
}

void Widget::set_position( const glm::vec2& pos )
{
	m_transform.pos = {pos, 0.f};
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
		_initialize_debug_rect();
	}
}

void Widget::set_anchor_point( const glm::vec2& anchor_point )
{
    m_anchor_point = anchor_point;
}

glm::vec2 Widget::convert_to_world_space( const glm::vec2& pos ) const
{
	return glm::vec4{ pos, 0.f, 1.f } * m_transform.transform;
}

glm::vec2 Widget::get_world_position( ) const
{
	return m_transform.get_world_position();
}

glm::vec2 Widget::get_local_position() const
{
	return m_transform.pos;
}

glm::vec2 Widget::get_left_top_world_position( ) const
{
    glm::vec2 pos = get_world_position( );

    return pos;
}

bool Widget::hit_test( const glm::vec2& point ) const
{
    return m_world_space_rect.hit_test( point );
}

void Widget::set_visible( bool visible )
{
    m_visible = visible;
}

bool Widget::is_visible( ) const
{
    return m_visible;
}

HAlign Widget::get_horizontal_align() const
{
    return m_horizontal_align;
}

void Widget::set_horizontal_align(HAlign align)
{
	m_horizontal_align = align;
}

VAlign Widget::get_vertical_align() const
{
	return m_vertical_align;
}

void Widget::set_vertical_align(VAlign align)
{
	m_vertical_align = align;
}

uint32_t Widget::get_camera_index() const
{
	if (!m_root) {
		/// TODO: need to replace 0 to INVALID camera index
		return 0; 
	}

	if (const auto camera = m_root->get_ui_camera()) {
		return camera->get_camera_index();
	}

	return 0;
}

void Widget::on_mouse_pressed( input::MouseButton btn, basic::int32 x, basic::int32 y )
{
    if (!m_world_space_rect.hit_test( {x, y} ))
    {
		return;
    }

    for (const auto child : m_children)
    {
        child->on_mouse_pressed( btn, x, y );
    }
}

void Widget::on_mouse_move( basic::int32 x, basic::int32 y )
{
	for (const auto child : m_children)
    {
        child->on_mouse_move( x, y );
    }
}

void Widget::on_key_pressed( input::KeyCode code, basic::int16 sym )
{
	for (const auto child : m_children)
    {
        child->on_key_pressed( code, sym );
    }
}

core::WidgetSystem* Widget::get_widget_system( ) const
{
	return m_root;
}

void Widget::_initialize_debug_rect()
{
	core::IEngine* engine = m_root->get_engine();
	
	if (IRender* render = engine->get_render())
	{
		if (!m_debug_rect)
		{
			m_debug_rect = render->create_object();
		}
		update_transform();

		m_debug_rect->update_color({ 0, 255, 0, 200 });
		m_debug_rect->set_camera_index(get_camera_index());
		m_debug_rect->update_mvp(m_transform.transform);
		build_rect(m_debug_rect, {0.f, 0.f}, m_world_space_rect.size);
	}
}

void Widget::update_transform()
{
	if (const Widget* parent = get_parent()) {

		apply_align(m_transform, parent->get_size());

		m_transform.update(parent->get_transform());
	}
	else {
		m_transform.update(nullptr);
	}

}

void Widget::apply_align(core::FTransform& transform, const glm::vec2& size) const
{
	const auto content_size = get_size();
	float x_align = 0;
	float y_align = 0;

	switch (m_horizontal_align)
	{
		case HAlign::Center:
			x_align = (size.x - content_size.x) / 2;
			break;
		case HAlign::Left:
			x_align = 0;
			break;
		case HAlign::Right:
			x_align = (size.x - content_size.x);
			break;
	}
	switch (m_vertical_align)
	{
		case VAlign::Center:
			y_align = (size.y - content_size.y) / 2;
			break;
		case VAlign::Top:
			y_align = size.y - content_size.y;
			break;
		case VAlign::Bottom:
			y_align = 0;
			break;
	}

	transform.pos = { x_align, y_align, 0.f };
}
