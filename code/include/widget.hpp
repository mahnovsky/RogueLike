#pragma once

#include "defines.hpp"

#include "input.hpp"

#include "generic_object.hpp"
#include "type_registration.hpp"
#include "generic_object.hpp"

class IEngine;

enum class Align
{
    Center,
    Left,
	Top,
    Right,
	Bottom
};

struct Rect
{
    glm::vec2 pos; // left-bottom
    glm::vec2 size;

	Rect() = default;
	Rect(const glm::vec2& _size)
		:pos()
		, size(_size)
	{}
    Rect( const glm::vec2& _pos, const glm::vec2& _size )
        :pos(_pos)
        ,size(_size)
    {}

    bool hit_test(const glm::vec2& point)
    {
		glm::vec2 rt = pos + size;
        return point.x >= pos.x && point.y >= pos.y &&
                point.x <= rt.x && pos.y <= rt.y;
    }
};

struct WidgetAction
{
    using Callback = void(*)(Widget*, void*);

    basic::String name;
    Callback callback;
    void* user_data;
};

class Widget 
{
public:
    Widget( core::WidgetSystem* widget );

    virtual ~Widget();

	//virtual void initialize() {}

	virtual void draw(IRender* render);

    virtual void add_child( Widget* node );

	virtual void remove_child(Widget* node);

    void remove_children();

    void remove_from_parent( );

    bool is_contains( Widget* child );

    bool get_child_index( Widget* node, uint32_t &out_index ) const;

    Widget* get_parent();

    Widget* get_child( uint32_t index );

    uint32_t get_child_count() const;

    void set_press_action(const basic::String& action_name);

    void set_position(const glm::vec2& pos);

    glm::vec2 get_size() const;

    void set_size(const glm::vec2& size);

    void set_anchor_point(const glm::vec2& anchor_point);

    glm::vec2 convert_to_world_space( const glm::vec2& pos ) const;

    glm::vec2 get_world_position() const;

	glm::vec2 get_local_position() const;

    glm::vec2 get_left_top_world_position() const;

    bool hit_test(const glm::vec2& point);

    void set_visible(bool visible);

    bool get_visible() const;

    Align get_align() const;

    void set_align(Align align);

    void set_picture(Texture* tex );

protected:
    virtual void on_mouse_pressed(input::MouseButton btn , basic::int32 x, basic::int32 y);

    virtual void on_mouse_move(basic::int32 x, basic::int32 y);

    virtual void on_key_pressed(input::KeyCode btn, basic::int16 sym);

    const ICamera* get_camera() const;

    core::WidgetSystem* get_widget_system();

	void _initialize_debug_rect();

protected:
    core::WidgetSystem* m_root;
	Rect m_world_space_rect;

    glm::vec2 m_local_pos;
    glm::vec2 m_anchor_point;
   
    Widget* m_parent;
    std::vector<Widget*> m_children;
    const ICamera* m_camera;
    
    basic::String m_press_action_name;
    bool m_visible;

    Align m_align;
    ResourceStorage* m_storage;
    IRenderObject* m_debug_rect;
	//class DrawingRect* m_debug_rect;
};
