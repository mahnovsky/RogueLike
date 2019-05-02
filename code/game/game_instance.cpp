#include "game_instance.hpp"

#include "render_common.hpp"
#include "texture.hpp"
#include "object_manager.hpp"
#include "root_widget.hpp"
#include "widget_list.hpp"
#include "widget_text.hpp"

#include <stdio.h>

GameInstance::GameInstance( Engine* engine, float width, float height )
    : m_engine( engine )
    , m_manager( NEW_OBJ(ObjectManager) )
    , m_rs( m_manager )
    , m_game_camera( NEW_OBJ( PerspectiveCamera, m_manager, 60.f, width / height, 1.f, 1000.f ) )
    , m_ui_camera( NEW_OBJ( OrthoCamera, m_manager, width, height, 0.f, 100.f ) )
    , m_back( )
    , m_btn( )
    , m_width( width )
    , m_height( height )
    , m_fps_text( nullptr )
    , m_mem_text( nullptr )
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
    RenderNode::remove_node( m_cow );

	SAFE_RELEASE(m_ui_root);
    SAFE_RELEASE(m_game_camera);
    SAFE_RELEASE(m_ui_camera);

	DELETE_OBJ(m_manager);
}

static void exit_action(Widget* w, void* ud)
{
    LOG("on widget clicked tag %d", w->get_tag());
    Engine* engine = static_cast<Engine*>(ud);

    engine->shutdown();
}

static void close_action(Widget* w, void* )
{
    LOG("on widget clicked tag %d", w->get_tag());
    if( w->get_parent() )
    {
        w->get_parent()->remove_from_parent();
    }
}

void open_menu_action(Widget* w, void* user_data)
{
    LOG("on widget clicked tag %d", w->get_tag());

    GameInstance* gi = static_cast<GameInstance*>(user_data);
    Texture* texture = gi->m_rs.get_resorce<Texture>( "SoM_Icon_2.png" );

    Widget* wnd = NEW_OBJ(WidgetList, gi->m_manager, {200, 300});
    wnd->set_tag(1);
    wnd->init( &gi->m_rs );
    wnd->set_position( {gi->m_width / 2, gi->m_height / 2} );
    wnd->set_anchor_point({0.5f, 0.5f});
    gi->m_ui_root->add_child(wnd);

    for(int i = 0; i < 3; ++i)
    {
        Widget* btn = NEW_OBJ(Widget, gi->m_manager, {200, 50});
        btn->set_tag(i);
        btn->set_press_action("wa_close");
        btn->init( &gi->m_rs );
        btn->set_picture( texture );
        wnd->add_child(btn);
    }

    WidgetText* text = NEW_OBJ(WidgetText, gi->m_manager, {200, 50});
    text->init( &gi->m_rs );
    text->set_text("exit");
    text->set_color({255, 0, 100, 255});
    text->set_press_action("wa_exit");
    text->set_tag(1);
    text->set_align( AlignV::Center );
    text->set_align( AlignH::Center );
    text->set_picture( texture );
    wnd->add_child(text);

    gi->m_ui_root->add_child(wnd);
}

void
GameInstance::init( )
{
    m_cam_pos = {10.f, 10.f, 10.f};
    m_cam_move_direction = glm::normalize( glm::vec3{0.f, 0.f, 0.f} - m_cam_pos );

    m_game_camera->init( m_cam_pos, {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f} );
    m_ui_camera->init( {0.f, 0.f, 0.f}, {}, {} );
    // m_ui_camera.init( {0.f, 0.f, 0.f}, {}, {} );

    Texture* texture = m_rs.get_resorce<Texture>( "SoM_Icon_2.png" );

    m_ui_root->init( &m_rs );


    WidgetAction wa_open_menu {"wa_open_menu", &open_menu_action, this};
    WidgetAction wa_exit {"wa_exit", &exit_action, m_engine};
    WidgetAction wa_close {"wa_close", &close_action, m_engine};
    m_ui_root->add_action( wa_exit );
    m_ui_root->add_action( wa_close );
    m_ui_root->add_action( wa_open_menu );

    //m_ui_root->add_press_callback(btn_cb);
    {
        WidgetList* wnd = NEW_OBJ(WidgetList, m_manager, {400.f, 200.f});

		wnd->init(&m_rs);
        m_fps_text = NEW_OBJ(WidgetText, m_manager, {200, 40});
        m_fps_text->init( &m_rs );
        m_fps_text->set_text("fps: ");
        m_fps_text->set_align( AlignH::Left );
        wnd->add_child(m_fps_text);

        m_mem_text = NEW_OBJ(WidgetText, m_manager, {400, 40});
        m_mem_text->init( &m_rs );
        m_mem_text->set_text("memory usage: ");
        m_mem_text->set_align( AlignH::Left );
        wnd->add_child(m_mem_text);

        WidgetText* menu_btn = NEW_OBJ(WidgetText, m_manager, {200, 40});

        menu_btn->init(&m_rs);
        menu_btn->set_text("MENU");
        menu_btn->set_align( AlignH::Center );
        menu_btn->set_picture( texture );
        wnd->set_press_action("wa_open_menu");
        wnd->add_child( menu_btn );

        m_ui_root->add_child(wnd);
    }

    ShaderProgram* shader = m_rs.get_resorce<ShaderProgram>( "texture" );

    m_back.init(shader, texture);
	m_back.set_size(2.f, 2.f);
    m_back.set_color( 255, 255, 50, 180 );

    shader = m_rs.get_resorce<ShaderProgram>( "texture_grid" );
    m_btn.init(shader, texture);
    m_btn.set_size(100.f, -100.f);
	m_btn.set_position({ 100.f, 100.f, 0.f });
    m_btn.set_frame_name("f_0");

    ShaderProgram* def_shader = m_rs.get_resorce<ShaderProgram>( "default" );
    Mesh m;
    if ( load_mesh( "meshes/cow.obj", m ) && def_shader )
    {
        m_cow = RenderNode::create_node( def_shader, nullptr );
		m_cow->set_camera( m_game_camera);
		m_cow->set_color( basic::Color{ 255, 255, 255, 255 });
        
		m_cow->init_node( &m.vb, &m.ib );
    }
}

void
GameInstance::draw( IRender* render )
{
    //m_cow->draw_node(  );
    m_back.draw( m_game_camera, render );
    m_btn.draw( m_ui_camera, render );

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
            m_fps_text->set_text( buff );
        }
    }

	basic::String::format(buff, BUFF_SIZE, "memory usage: %u", basic::get_memory_usage());
    m_mem_text->set_text(buff);
}
