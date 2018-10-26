#include "game_instance.hpp"


GameInstance::GameInstance(Engine* engine, float aspect)
    : m_engine( engine )
    , m_game_camera( 45.f, aspect, 0.f, 100.f )
    , m_back()
{}

void GameInstance::init()
{
    m_cam_pos = {4.f, 5.f, 3.f};
    m_cam_move_direction = glm::normalize( glm::vec3{0.f, 0.f, 0.f} - m_cam_pos );

    m_game_camera.init( {4.f, 5.f, 3.f}, {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f} );

    m_back.set_color( 255, 255, 255, 255 );
    m_back.set_size( 2.f, 2.f );
    m_back.init("my.bmp");
}

void GameInstance::draw( IRender* render )
{
    m_back.draw( &m_game_camera, render );
}

void GameInstance::frame( float delta )
{
    LOG( "delta %f", delta );
    static float time;
    time += delta;

    glm::vec3 dpos = m_cam_move_direction * (4.f * delta * glm::sin( time ) );
    m_cam_pos += dpos;

    m_game_camera.set_position( m_cam_pos );
}

void GameInstance::cleanup()
{

}

