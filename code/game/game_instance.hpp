#pragma once

#include "camera.hpp"
#include "engine.hpp"
#include "font.hpp"
#include "shader.hpp"
#include "sprite.hpp"
#include "text.hpp"
#include "resource_storage.hpp"
#include "line.hpp"


class GameInstance
{
public:
    GameInstance( Engine* engine, float width, float height );
    ~GameInstance( );

    void init( );

    void draw( IRender* render );

    void frame( float delta );

    void cleanup( );

private:
    void print_fps();

private:
    Engine* m_engine;

    ResourceStorage m_rs;
    PerspectiveCamera m_game_camera;
    OrthoCamera m_ui_camera;

    Sprite m_back;
    Sprite m_btn;
    RenderObject m_model;
    glm::vec3 m_cam_pos;
    glm::vec3 m_cam_move_direction;

    float m_width;
    float m_height;

    Text m_fps_text;
	Text m_mem_text;
    Line m_line;
    RenderNode* m_cow;
};
