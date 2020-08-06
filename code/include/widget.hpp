#pragma once

#include "defines.hpp"

#include "object.hpp"
#include "input.hpp"

#include "generic_object.hpp"
#include "type_registration.hpp"
#include "generic_object_manager.hpp"


enum class AlignH
{
    Center,
    Left,
    Right
};

enum class AlignV
{
    Center,
    Top,
    Bottom
};

struct Rect
{
    glm::vec2 left_top;
    glm::vec2 right_bottom;

    Rect()
        :left_top()
        ,right_bottom()
    {}

    Rect( const glm::vec2& lt, const glm::vec2& tb )
        :left_top(lt)
        ,right_bottom(tb)
    {}

    void update(const glm::vec2& pos, const glm::vec2& size)
    {
        left_top = pos;
        right_bottom = pos + size;
    }

    bool hit_test(const glm::vec2& pos)
    {
        return pos.x >= left_top.x && pos.y >= left_top.y &&
                pos.x <= right_bottom.x && pos.y <= right_bottom.y;
    }
};

struct WidgetAction
{
    using Callback = void(*)(Widget*, void*);

    basic::String name;
    Callback callback;
    void* user_data;
};

class Widget : public IGenericObject
{
public:
    GENERIC_OBJECT_IMPL(Widget, NS_COMPONENT_TYPE);

    Widget( GenericObjectManager* manager );

    ~Widget() override;

    virtual void init(ResourceStorage* storage);

    virtual void draw();

    virtual void add_child( Widget* node );

	virtual void remove_child(Widget* node);

    void remove_children();

    void remove_from_parent( );

    bool is_contains( Widget* child );

    bool get_child_index( Widget* node, basic::uint32 &out_index ) const;

    Widget* get_parent();

    Widget* get_child( basic::uint32 index );

    basic::uint32 get_child_count() const;

    void set_press_action(const basic::String& action_name);

    void set_position(const glm::vec2& pos);

    glm::vec2 get_size() const;

    void set_size(const glm::vec2& size);

    void set_anchor_point(const glm::vec2& anchor_point);

    glm::vec2 convert_to_world_space( const glm::vec2& pos ) const;

    glm::vec2 get_world_position() const;

    glm::vec2 get_left_top_world_position() const;

    bool hit_test(const glm::vec2& point);

    void set_visible(bool visible);

    bool get_visible() const;

    AlignH get_horizontal_align() const;

    void set_align(AlignH horizontal);

    AlignV get_vertical_align() const;

    void set_align(AlignV vertical);

    void set_picture(Texture* tex );

protected:
    virtual void on_mouse_pressed(input::MouseButton btn , basic::int32 x, basic::int32 y);

    virtual void on_mouse_move(basic::int32 x, basic::int32 y);

    virtual void on_key_pressed(input::KeyCode btn, basic::int16 sym);

    void update_rect();

    glm::mat4 get_matrix() const;

    RenderNode* get_view();

    ICamera* get_camera();

	void update_mat();

    RootWidget* get_root();

private:
    GenericObjectManager* m_object_manager;
	glm::mat4 m_mat;
    glm::vec2 m_pos;
    glm::vec2 m_size;
    glm::vec2 m_anchor_point;
    Rect m_rect;
    Widget* m_parent;
    basic::Vector<Widget*> m_children;
    ICamera* m_camera;
    
    basic::String m_press_action_name;
    bool m_visible;

protected:
    AlignH m_horizontal;
    AlignV m_vertical;
    ResourceStorage* m_storage;
    RootWidget* m_root;
};
