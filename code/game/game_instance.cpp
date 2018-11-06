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
	m_cam_pos = { 3.f, 3.f, 3.f };
    m_cam_move_direction = glm::normalize( glm::vec3{0.f, 0.f, 0.f} - m_cam_pos );

    m_game_camera.init(m_cam_pos*3.f, {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f} );
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

	if (m_font.init("arial.ttf"))
	{
		m_text.set_font(&m_font);
		m_text.set_text("hello world");
		m_text.set_scale(0.1f);
		//m_text.set_position({ 10.f, 10.f, 0.f });
	}
}

void GameInstance::draw( IRender* render )
{
    m_back.draw( &m_game_camera, render );
    //m_btn.draw( &m_ui_camera, render );
    m_text.draw( render, &m_game_camera );
}

void GameInstance::frame( float delta )
{
    static float time;
    time += delta;

    glm::vec3 dpos = m_cam_move_direction * (4.f * delta * glm::sin( time ) );
    m_cam_pos += dpos;

    //m_game_camera.set_position( m_cam_pos );
	auto pos = m_back.get_position();
	pos.x += (0.1f * delta);
	pos.y += (0.1f * delta);

	m_back.set_position(pos);
}

void GameInstance::cleanup()
{

}

