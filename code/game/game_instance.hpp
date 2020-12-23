#pragma once

#include "defines.hpp"

#include "camera.hpp"
#include "engine.hpp"
#include "input.hpp"
#include "resource_storage.hpp"
#include "generic_object_manager.hpp"
#include "system_manager.hpp"

class GameInstance : public input::InputListener
{
public:
    GameInstance( Engine* engine, float width, float height );
    ~GameInstance( );

    void init( );

    void draw( IRender* render ) const;

    void frame( float delta );

    void key_pressed( input::KeyCode code, basic::int16 key ) override;

	void on_mouse_event(const input::MouseEvent& mouse_event) override;

    friend void open_menu_action( Widget* w, void* user_data );

private:
    void print_fps( int objects) const;

	void set_camera_to_entity(const Entity* ent);

private:
    Engine* m_engine;
    GenericObjectManager* m_object_manager;
    core::ResourceStorage* m_rs;
    PerspectiveCamera* m_game_camera;
    
    glm::vec3 m_cam_pos;
    glm::vec3 m_cam_move_direction;

    float m_width;
    float m_height;

    WidgetText* m_fps_text;
    WidgetText* m_mem_text;
    RenderNode* m_cow;
    core::WidgetSystem* m_ui_root;
	EcsManager* m_ecs;
    class RenderSystem* m_render_system;

    Entity* m_player;

	bool m_is_menu_opened = false;

	DrawingRect* m_selection_rect;

	bool m_selection_state = false;
	glm::vec2 m_start_pos;
	glm::vec2 m_end_pos;

	class MoveSystem* m_move_system;
	int m_ent_index = 0;
};
