#include "game_instance.hpp"


GameInstance::GameInstance(Engine* engine, float width, float height)
    : m_engine( engine )
    , m_texture_cache()
    , m_game_camera( 45.f, width / height, 0.f, 100.f )
    , m_ui_camera( width, height, 0.f, 100.f )
    , m_back()
    , m_btn()
    , m_width( width )
    , m_height( height )
{}

void GameInstance::init()
{
    m_cam_pos = {4.f, 5.f, 3.f};
    m_cam_move_direction = glm::normalize( glm::vec3{0.f, 0.f, 0.f} - m_cam_pos );

    m_game_camera.init( {4.f, 5.f, 3.f}, {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f} );
    m_ui_camera.init( { m_width / 2, m_height / 2, 0.f}, {}, {} );

    TextureHandle handle;
    if( m_texture_cache.load_texture( "my.bmp", handle ) )
    {
        Texture* texture = m_texture_cache.get_texture( handle );

        m_back.init( texture );
        m_back.set_size( 2.f, 2.f );

        m_btn.init( texture );
        m_btn.set_size( 100.f, 100.f );
        m_btn.set_position( { 100.f, 100.f, 0.f } );
    }
    m_font.init( "arial.ttf" );
}

void GameInstance::draw( IRender* render )
{
    m_back.draw( &m_game_camera, render );
    m_btn.draw( &m_ui_camera, render );
    m_font.draw( &m_ui_camera, render, "Text test abcdefghijk" );
}

void GameInstance::frame( float delta )
{
    static float time;
    time += delta;

    glm::vec3 dpos = m_cam_move_direction * (4.f * delta * glm::sin( time ) );
    m_cam_pos += dpos;

    //m_game_camera.set_position( m_cam_pos );
}

void GameInstance::cleanup()
{

}

