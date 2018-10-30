#pragma once

#include "engine.hpp"
#include "sprite.hpp"
#include "camera.hpp"
#include "texture_cache.hpp"

class GameInstance
{
public:
    GameInstance(Engine* engine, float width, float height);

    void init();

    void draw( IRender* render );

    void frame( float delta );

    void cleanup();

private:
    Engine* m_engine;

    TextureCache m_texture_cache;
    PerspectiveCamera m_game_camera;
    OrthoCamera m_ui_camera;

    Sprite m_back;
    Sprite m_btn;
    glm::vec3 m_cam_pos;
    glm::vec3 m_cam_move_direction;

    float m_width;
    float m_height;
};



