#pragma once

#include "engine.hpp"
#include "sprite.hpp"
#include "camera.hpp"

class GameInstance
{
public:
    GameInstance(Engine* engine, float aspect);

    void init();

    void draw( IRender* render );

    void frame( float delta );

    void cleanup();

private:
    Engine* m_engine;
    PerspectiveCamera m_game_camera;
    Sprite m_back;    
    glm::vec3 m_cam_pos;
    glm::vec3 m_cam_move_direction;
};



