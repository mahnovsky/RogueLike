#pragma once

#include "defines.hpp"
#include "input.hpp"
#include "type_registration.hpp"
#include "ftransform.h"

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
		: pos()
		, size(_size)
	{
	}

	Rect(const glm::vec2& _pos, const glm::vec2& _size)
		: pos(_pos)
		, size(_size)
	{
	}

	bool hit_test(const glm::vec2& point) const
	{
		glm::vec2 rt = pos + size;
		return point.x >= pos.x && point.x <= rt.x && point.y >= pos.y && point.y <= rt.y;
	}
};

class Widget
{
public:
	Widget(core::WidgetSystem* widget);

	virtual ~Widget();

	virtual void update();

	virtual void draw(IRender* render);

	virtual void add_child(Widget* node);

	virtual void remove_child(Widget* node);

	const core::FTransform* get_transform() const;

	void remove_children();

	void remove_from_parent();

	bool is_contains(Widget* child);

	bool get_child_index(Widget* node, uint32_t& out_index) const;

	Widget* get_parent() const;

	Widget* get_child(uint32_t index) const;

	uint32_t get_child_count() const;

	void set_position(const glm::vec2& pos);

	glm::vec2 get_size() const;

	void set_size(const glm::vec2& size);

	void set_anchor_point(const glm::vec2& anchor_point);

	glm::vec2 convert_to_world_space(const glm::vec2& pos) const;

	glm::vec2 get_world_position() const;

	glm::vec2 get_local_position() const;

	glm::vec2 get_left_top_world_position() const;

	bool hit_test(const glm::vec2& point) const;

	void set_visible(bool visible);

	bool is_visible() const;

	Align get_align() const;

	virtual void set_align(Align align);

	uint32_t get_camera_index() const;

	virtual void on_mouse_pressed(input::MouseButton btn, basic::int32 x, basic::int32 y);

	virtual void on_mouse_move(basic::int32 x, basic::int32 y);

	virtual void on_key_pressed(input::KeyCode btn, basic::int16 sym);

protected:
	core::WidgetSystem* get_widget_system() const;

	void _initialize_debug_rect();

	virtual void update_transform();

protected:
	core::WidgetSystem* m_root;
	core::FTransform m_transform;
	Rect m_world_space_rect;
	glm::vec2 m_anchor_point;
	Widget* m_parent;
	std::vector<Widget*> m_children;
	bool m_visible;
	Align m_align;
	ResourceStorage* m_storage;
	IRenderObject* m_debug_rect;
};
