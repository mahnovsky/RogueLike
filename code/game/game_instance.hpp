#pragma once

#include "defines.hpp"

#include "igame_instance.hpp"
#include "camera.hpp"
#include "engine.hpp"
#include "input.hpp"
#include "resource_storage.hpp"
#include "system_manager.hpp"

class GameInstance 
    : public core::IGameInstance
    , public input::InputListener
{
public:
    GameInstance( IEngine* engine, float width, float height );
    ~GameInstance( ) override;

    void initialize() override;

    void draw( IRender* render ) override;

    void update( float delta ) override;

    void key_pressed( input::KeyCode code, basic::int16 key ) override;

	void on_mouse_event(const input::MouseEvent& mouse_event) override;

    //friend void open_menu_action( Widget* w, void* user_data );

	void initialize_cams();

	void initialize_ui();

private:
    void print_fps(int objects) const;

	void set_camera_to_entity(const Entity* ent);

private:
    IEngine* m_engine;
    core::ResourceStorage* m_rs;
    PerspectiveCamera* m_game_camera;
    
    glm::vec3 m_cam_pos;
    glm::vec3 m_cam_move_direction;

    float m_width;
    float m_height;

    WidgetText* m_fps_text;
    WidgetText* m_mem_text;
    RenderNode* m_cow;
    core::WidgetSystem* m_widget_system;
	EntityComponentManager* m_ecs;
    class RenderSystem* m_render_system;

    Entity* m_player;

	bool m_is_menu_opened = false;

	//DrawingRect* m_selection_rect;
    IRenderObject* m_selection_rect;

	bool m_selection_state = false;
	glm::vec2 m_start_pos;
	glm::vec2 m_end_pos;

	class MoveSystem* m_move_system;
	int m_ent_index = 0;
};
