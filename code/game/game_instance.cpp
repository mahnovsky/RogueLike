#include "game_instance.hpp"

#include "render_common.hpp"
#include "texture.hpp"
#include "object_manager.hpp"
#include "root_widget.hpp"
#include "widget_list.hpp"

#include <stdio.h>

GameInstance::GameInstance( Engine* engine, float width, float height )
    : m_engine( engine )
    , m_manager( NEW_OBJ(ObjectManager) )
    , m_rs(m_manager)
    , m_game_camera( NEW_OBJ( PerspectiveCamera, m_manager, 60.f, width / height, 1.f, 1000.f ) )
    , m_ui_camera( NEW_OBJ( OrthoCamera, m_manager, width, height, 0.f, 100.f ) )
    , m_back( )
    , m_btn( )
    , m_width( width )
    , m_height( height )
	, m_fps_text("arial.ttf")
	, m_mem_text("arial.ttf")
    , m_ui_root( NEW_OBJ( RootWidget, engine, m_manager ) )
{
    m_game_camera->set_name("game_camera");
    m_game_camera->retain();

    m_ui_camera->set_name("ui_camera");
    m_ui_camera->retain();

    m_ui_root->retain();
}

GameInstance::~GameInstance( )
{
    remove_node( m_cow );

	SAFE_RELEASE(m_ui_root);
    SAFE_RELEASE(m_game_camera);
    SAFE_RELEASE(m_ui_camera);

	DELETE_OBJ(m_manager);
}

void click_print(Widget* w, void* ud)
{
    static int count;

    LOG("on widget clicked %d, tag %d", ++count, w->get_tag());
    Engine* engine = static_cast<Engine*>(ud);
    switch (w->get_tag()) {
    case 1:
        LOG("1 btn pressed");
		//w->remove_from_parent();
		w->get_parent()->remove_from_parent();
        //engine->shutdown();
        break;
    default:
        break;
    }
}

void
GameInstance::init( )
{
    m_cam_pos = {10.f, 10.f, 10.f};
    m_cam_move_direction = glm::normalize( glm::vec3{0.f, 0.f, 0.f} - m_cam_pos );

    m_game_camera->init( m_cam_pos, {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f} );
    m_ui_camera->init( {0.f, 0.f, 0.f}, {}, {} );
    // m_ui_camera.init( {0.f, 0.f, 0.f}, {}, {} );

    m_ui_root->init( &m_rs );
    WidgetCallback btn_cb {&click_print, m_engine};
    m_ui_root->add_press_callback(btn_cb);
    {
        Widget* wnd = NEW_OBJ(WidgetList, m_manager, {200, 300});
        wnd->set_tag(1);
        //wnd->add_press_callback(btn_cb);
        wnd->init( &m_rs );
        wnd->set_position( {m_width / 2, m_height / 2} );
        wnd->set_anchor_point({0.5f, 0.5f});
        m_ui_root->add_child(wnd);

        for(int i = 0; i < 3; ++i)
        {
            Widget* btn = NEW_OBJ(Widget, m_manager, {200, 50});
            btn->set_tag(i);
            btn->add_press_callback(btn_cb);
            btn->init( &m_rs );
            //btn->set_position( {0.f, 10.f} );
            wnd->add_child(btn);
        }
    }

    ShaderProgram* shader = m_rs.get_resorce<ShaderProgram>( "texture" );
	Texture* texture = m_rs.get_resorce<Texture>( "SoM_Icon_2.png" );

    m_back.init(shader, texture);
	m_back.set_size(2.f, 2.f);
    m_back.set_color( 255, 255, 50, 180 );

    m_btn.init(shader, texture);
    m_btn.set_size(100.f, -100.f);
	m_btn.set_position({ 100.f, 100.f, 0.f });

    m_fps_text.init(&m_rs);
    m_fps_text.set_text( "fps: " );
    m_fps_text.set_position( {20.f, m_height - 40.f, 0.f} );
	m_fps_text.set_scale(0.8f);

    m_mem_text.init(&m_rs);
	m_mem_text.set_text("memory usage: ");
	m_mem_text.set_position({ 20.f, m_height - 60.f, 0.f });
	m_mem_text.set_scale(0.8f);

    ShaderProgram* def_shader = m_rs.get_resorce<ShaderProgram>( "default" );
    Mesh m;
    if ( load_mesh( "meshes/cow.obj", m ) && def_shader )
    {
        m_cow = create_node( def_shader, nullptr );
        m_cow->camera = m_game_camera;
        m_cow->color = basic::Color{255, 255, 255, 255};
        init_node( m_cow, &m.vb, &m.ib );
    }
}

void
GameInstance::draw( IRender* render )
{
    draw_node( m_cow );
    m_back.draw( m_game_camera, render );
    m_btn.draw( m_ui_camera, render );
    m_fps_text.draw( render, m_ui_camera );
    m_mem_text.draw( render, m_ui_camera );

    m_ui_root->draw();
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
    #define BUFF_SIZE 256
    static basic::char_t buff[BUFF_SIZE];
    static basic::uint32 fps;

    if( fps != m_engine->get_fps() )
    {
        fps = m_engine->get_fps();
        if( basic::String::format( buff, BUFF_SIZE, "fps: %u", fps ) )
        {
            m_fps_text.set_text( buff );
        }
    }

	basic::String::format(buff, BUFF_SIZE, "memory usage: %u", basic::get_memory_usage());
	m_mem_text.set_text(buff);
}
