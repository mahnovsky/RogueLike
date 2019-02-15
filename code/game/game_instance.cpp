#include "game_instance.hpp"
#include "render_common.hpp"
#include "texture.hpp"

#include <stdio.h>

GameInstance::GameInstance( Engine* engine, float width, float height )
    : m_engine( engine )
    , m_game_camera( 60.f, width / height, 1.f, 1000.f )
    , m_ui_camera( width, height, 0.f, 100.f )
    , m_back( )
    , m_btn( )
    , m_width( width )
    , m_height( height )
{
}

GameInstance::~GameInstance( )
{
}

void
GameInstance::init( )
{
    m_cam_pos = {10.f, 10.f, 10.f};
    m_cam_move_direction = glm::normalize( glm::vec3{0.f, 0.f, 0.f} - m_cam_pos );

    m_game_camera.init( m_cam_pos, {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f} );
    m_ui_camera.init( {m_width / 2, m_height / 2, 0.f}, {}, {} );
    // m_ui_camera.init( {0.f, 0.f, 0.f}, {}, {} );

    ShaderProgram* shader = m_rs.get_resorce<ShaderProgram>( "texture" );
	Texture* texture = m_rs.get_resorce<Texture>( "SoM_Icon_2.png" );

	m_back.init(shader, texture);
	m_back.set_size(2.f, 2.f);
	//m_back.set_color( 255, 255, 255, 20 );

	m_btn.init(shader, texture);
	m_btn.set_size(100.f, 100.f);
	m_btn.set_position({ 100.f, 100.f, 0.f });

    ShaderProgram* text_shader = m_rs.get_resorce<ShaderProgram>( "text" );

    if ( text_shader && m_font.init( "arial.ttf", text_shader ) )
    {
        m_text.set_font( &m_font );
        m_text.set_text( "hello world" );
        m_text.set_position( {20.f, m_height - 40.f, 0.f} );
    }

    ShaderProgram* def_shader = m_rs.get_resorce<ShaderProgram>( "default" );
    Mesh m;
    if ( load_mesh( "cow.obj", m ) && def_shader )
    {
        m_model.set_index_buffer( std::move( m.ib ) );
        m_model.set_vertex_buffer( std::move( m.vb ) );
        m_model.set_shader( def_shader );
        //m_model.set_texture( texture );
        //m_model.get_transform( )->set_scale( {10.f, 10.f, 10.f} );
        m_model.init( );
    }
}

void
GameInstance::draw( IRender* render )
{
    m_model.draw( render, &m_game_camera );
    m_back.draw( &m_game_camera, render );
    m_btn.draw( &m_ui_camera, render );
    m_text.draw( render, &m_ui_camera );
}

void
GameInstance::frame( float delta )
{
    static float time;
    time += delta;

    glm::vec3 dpos = m_cam_move_direction * ( 4.f * delta * glm::sin( time ) );
    m_cam_pos += dpos;

    // m_game_camera.set_position( m_cam_pos );
    auto pos = m_back.get_position( );
    pos.x += ( 0.8f * delta );
    pos.y += ( 0.8f * delta );

    m_back.set_position( pos );

    print_fps();

    //float angle = m_btn.get_angle();

    //m_btn.set_angle( angle + 0.03f );
}

void
GameInstance::cleanup( )
{
}

void GameInstance::print_fps()
{
    #define BUFF_SIZE 32
    static basic::char_t buff[BUFF_SIZE];
    static basic::uint32 fps;

    if( fps != m_engine->get_fps() )
    {
        fps = m_engine->get_fps();
        if( basic::String::format( buff, BUFF_SIZE, "fps: %u", fps ) )
        {
            m_text.set_text( buff );
        }
    }
}
