#include "game_instance.hpp"

#include "material.hpp"
#include "object_manager.hpp"
#include "render_common.hpp"
#include "root_widget.hpp"
#include "texture.hpp"
#include "widget_list.hpp"
#include "widget_text.hpp"

#include "entity.hpp"
#include "entity_component_system.hpp"

#include "render.hpp"
#include "render_system.hpp"
#include "static_mesh.hpp"

#include <stdio.h>
#include <ctime>

struct Test
{
    // DECLARE_TYPE_UID;
};

GameInstance::GameInstance( Engine* engine, float width, float height )
    : m_engine( engine )
    , m_manager( NEW_OBJ( ObjectManager ) )
    , m_rs( nullptr )
    , m_game_camera( NEW_OBJ( PerspectiveCamera, m_manager, 60.f, width / height, 1.f, 1000.f ) )
    , m_ui_camera( NEW_OBJ( OrthoCamera, m_manager, width, height, 0.f, 100.f ) )
    , m_width( width )
    , m_height( height )
    , m_fps_text( nullptr )
    , m_mem_text( nullptr )
    , m_ui_root( NEW_OBJ( RootWidget, engine, m_manager ) )
	, m_player(nullptr)
{
	m_rs = &m_engine->get_rs();

    m_game_camera->set_name( "game_camera" );
    m_game_camera->retain( );

    m_ui_camera->set_name( "ui_camera" );
    m_ui_camera->retain( );

    m_ui_root->retain( );
}

GameInstance::~GameInstance( )
{
    SAFE_RELEASE( m_ui_root );
    SAFE_RELEASE( m_game_camera );
    SAFE_RELEASE( m_ui_camera );

    DELETE_OBJ( m_ecs );
    DELETE_OBJ( m_manager );
}

static void
exit_action( Widget* w, void* ud )
{
    LOG( "on widget clicked tag %d", w->get_tag( ) );
    Engine* engine = static_cast< Engine* >( ud );

    engine->shutdown( );
}

static void
close_action( Widget* w, void* user_data )
{
    LOG( "on widget clicked tag %d", w->get_tag( ) );
    if ( w->get_parent( ) )
    {
        w->get_parent( )->remove_from_parent( );
    }
}

basic::Color g_ui_color{ 255, 0, 100, 255 };

void open_menu_action( Widget* w, void* user_data )
{
    LOG( "on widget clicked tag %d", w->get_tag( ) );

    auto gi = static_cast< GameInstance* >( user_data );
	if(gi->m_is_menu_opened)
	{
		return;
	}
	gi->m_is_menu_opened = true;

    Texture* texture = gi->m_rs->get_resorce< Texture >( "btn.png" );

    Widget* wnd = NEW_OBJ( WidgetList, gi->m_manager, {200, 300} );
    wnd->set_tag( 1 );
    wnd->init( gi->m_rs );
    wnd->set_position( {gi->m_width / 2, gi->m_height / 2} );
    wnd->set_anchor_point( {0.5f, 0.5f} );
	
    gi->m_ui_root->add_child( wnd );

    for ( int i = 0; i < 3; ++i )
    {
        Widget* btn = NEW_OBJ( Widget, gi->m_manager, {200, 50} );
        btn->set_tag( i );
        btn->set_press_action( "wa_close" );
        btn->init( gi->m_rs );
        btn->set_picture( texture );
        wnd->add_child( btn );
    }

    WidgetText* text = NEW_OBJ( WidgetText, gi->m_manager, {200, 50} );
    text->init( gi->m_rs );
    text->set_text( "exit" );
    text->set_color( g_ui_color );
    text->set_press_action( "wa_exit" );
    text->set_tag( 1 );
    text->set_align( AlignV::Center );
    text->set_align( AlignH::Center );
    text->set_picture( texture );
    wnd->add_child( text );

    gi->m_ui_root->add_child( wnd );
}

struct MoveComponent : public IComponent
{
    DECLARE_COMPONENT( MoveComponent )

    float angle_speed;
    float move_speed;
    glm::vec3 move_direction;
};

class MoveSystem : public ISystem
{
public:
    MoveSystem( EntityComponentSystem* ecs )
        : m_ecs( ecs )
    {}


    void
    on_component_event( Entity* ent, basic::uint32 component_id, ComponentAction act ) override
    {
        if ( component_id == m_ecs->get_component_id< TransformComponent >( ) )
        {
            if ( act == ComponentAction::Attached )
            {
                TransformComponent* tc = ent->get_component< TransformComponent >( );

                if ( tc )
                {
                    ASSERT( ent );
                    m_transforms.push( {ent, tc} );
                }
            }
        }
    }

    void initialize( )
    {
        m_ecs->registry_component< MoveComponent >( "MoveComponent" );
    }

    void
    start( ) override
    {
    }

    void update( float dt ) override
    {
        // return;
        const basic::Vector< MoveComponent* >& mcs = m_ecs->get_components< MoveComponent >( );

        for ( auto mc : mcs )
        {
            TransformComponent* tc;
            if ( mc->entity && ( tc = mc->entity->get_component< TransformComponent >( ) ) )
            {
                auto pos = tc->tr.get_position( );
                pos += ( mc->move_direction * mc->move_speed * dt );
                tc->tr.set_position( pos );

                glm::vec3 angles = tc->tr.get_euler_angles( );
                angles.y += ( mc->angle_speed * dt );
                tc->tr.set_euler_angles( angles );
            }
        }
    }

public:
    EntityComponentSystem* m_ecs;
    basic::Vector< basic::Pair< Entity*, TransformComponent* > > m_transforms;
};

static float
rnd( )
{
    return static_cast< float >( rand( ) ) / RAND_MAX;
}

Entity*
make_ent(EntityComponentSystem* ecs, const char* mesh, const char* shader, const char* texture = nullptr )
{
    Entity* ent = ecs->create( );

    auto tr = ent->add_component< TransformComponent >( );

    float v = 10.f;
    glm::vec3 pos{v * rnd( ), 0.f, v * rnd( )};

    tr->tr.set_position( pos );
    const float scale = rnd( ) * 2.5f;
    tr->tr.set_scale( {scale, scale, scale} );

    auto rc = ent->add_component< RenderComponent >( );

	rc->set_resource_name(RenderResourceType::ShaderProgram, shader);
	rc->set_resource_name(RenderResourceType::StaticMesh, mesh);
	//rc->set_mesh_load_settings()
	if (texture)
		rc->set_resource_name(RenderResourceType::Texture, texture);

    auto mc = ent->add_component< MoveComponent >( );
    mc->angle_speed = rnd( );
    mc->move_speed = rnd( ) * 2.f;
    mc->move_direction = {rnd( ), 0.f, rnd( )};
    // ecs->emit( ent, TransformComponent::TYPE_UID, ComponentAction::Updated );

    //RenderSystem::load_component( rc, m );

    return ent;
}

void
GameInstance::init( )
{
    srand( time( nullptr ) );
	Texture* texture = m_rs->get_resorce< Texture >("btn.png");

    ShaderProgram* shader = m_rs->get_resorce< ShaderProgram >( "texture" );

    m_ecs = NEW_OBJ( EntityComponentSystem );

    MoveSystem* ms = m_ecs->create_system< MoveSystem >( );
    ms->initialize( );

    m_render_system = m_ecs->create_system< RenderSystem >( );
    m_render_system->initialize( m_engine->get_render(), m_ecs, m_game_camera );

    m_cam_pos = {10.f, 10.f, 10.f};
    m_cam_move_direction = glm::normalize( glm::vec3{0.f, 0.f, 0.f} - m_cam_pos );

    m_game_camera->init( m_cam_pos, {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f} );
    m_ui_camera->init( {0.f, 0.f, 0.f}, {}, {} );
    // m_ui_camera.init( {0.f, 0.f, 0.f}, {}, {} );

    m_ui_root->init( m_rs );

    WidgetAction wa_open_menu{"wa_open_menu", &open_menu_action, this};
    WidgetAction wa_exit{"wa_exit", &exit_action, m_engine};
    WidgetAction wa_close{"wa_close", &close_action, m_engine};
    m_ui_root->add_action( wa_exit );
    m_ui_root->add_action( wa_close );
    m_ui_root->add_action( wa_open_menu );
	m_ui_root->bind_key_action(input::KeyCode::Esc, wa_open_menu.name);
    {
        WidgetList* wnd = NEW_OBJ( WidgetList, m_manager, {400.f, 200.f} );

        wnd->init( m_rs );
        m_fps_text = NEW_OBJ( WidgetText, m_manager, {200, 40} );
        m_fps_text->init( m_rs );
        m_fps_text->set_text( "fps: " );
		m_fps_text->set_color(g_ui_color);
        m_fps_text->set_align( AlignH::Left );
        wnd->add_child( m_fps_text );

        m_mem_text = NEW_OBJ( WidgetText, m_manager, {400, 40} );
        m_mem_text->init( m_rs );
        m_mem_text->set_text( "memory usage: " );
		m_mem_text->set_color(g_ui_color);
        m_mem_text->set_align( AlignH::Left );
        wnd->add_child( m_mem_text );

        WidgetText* menu_btn = NEW_OBJ( WidgetText, m_manager, {200, 40} );

        menu_btn->init( m_rs );
        menu_btn->set_text( "MENU" );
		menu_btn->set_color(g_ui_color);
        menu_btn->set_align( AlignH::Center );
        menu_btn->set_picture( texture );
        wnd->set_press_action( "wa_open_menu" );
        wnd->add_child( menu_btn );

        m_ui_root->add_child( wnd );
    }
    
	for (int i = 0; i < 5; ++i)
	{
		auto cow = make_ent(m_ecs, "meshes/cube.obj", "default");
		auto rc = cow->get_component<RenderComponent>();
		if(rc)
		{
			rc->set_color({ 128, 0, 55, 255 });
		}
	}

	m_player = make_ent(m_ecs, "meshes/cube.obj", "default");
	/*if (m_player)
	{
		auto mc = m_player->get_component< MoveComponent >();
		if (mc)
		{
			mc->move_speed = 0.f;
			mc->angle_speed = 0.f;
		}
		auto tr = m_player->get_component< TransformComponent >();
		if (tr)
		{
			tr->tr.set_euler_angles({ 0.f, 0.f, 0.f });
			tr->tr.set_position({ 10.f, 0.f, 10.f });
			tr->tr.set_scale({ 3.5f, 3.5f, 3.5f });
		}
		auto rc = m_player->get_component<RenderComponent>();
		rc->set_color({ 130, 0, 80, 255 });
	}*/

	Entity* plane_ent = make_ent(m_ecs, "meshes/plane.obj", "default_texture", "Chess_Board.png");
	if (plane_ent)
	{
		auto trc = plane_ent->get_component< TransformComponent >();
		trc->tr.set_scale({ 100.f, 1.f, 100.f });

		auto move_comp = plane_ent->get_component< MoveComponent >();
		move_comp->move_speed = 0.f;
		move_comp->angle_speed = 0.f;
	}
    

    m_engine->get_input( )->add_listener( this );

    m_ecs->start( );
}

void
GameInstance::draw( IRender* ) const
{
    m_render_system->draw( m_ecs );

    m_ui_root->draw( );
}

void
GameInstance::frame( float delta ) const
{
    m_ecs->update( delta );

    print_fps( );

    if ( m_player )
    {
        auto mc = m_player->get_component< MoveComponent >( );
        auto tr = m_player->get_component< TransformComponent >( );

		if (mc && tr)
		{
			mc->move_speed = 0.f;
			mc->angle_speed = 0.f;

			input::Input* input = m_engine->get_input();

			const float vel = 60.f;
			const float angle_vel = 5.f;

			if( input->is_key_pressed(input::KeyCode::A) )
				mc->angle_speed = angle_vel;
			if( input->is_key_pressed(input::KeyCode::D) )
				mc->angle_speed = -angle_vel;
			if( input->is_key_pressed(input::KeyCode::W) )
				mc->move_speed = vel;
			if( input->is_key_pressed(input::KeyCode::S) )
				mc->move_speed = -vel;
			if( input->is_key_pressed(input::KeyCode::Space) )
			{
				auto pos = tr->tr.get_position();
				pos.y += (vel * delta);
				tr->tr.set_position(pos);
			}
			if(input->is_key_pressed(input::KeyCode::E))
			{
				auto pos = tr->tr.get_position();
				pos.y -= (vel * delta);
				tr->tr.set_position(pos);
			}


			auto pos = tr->tr.get_position();
			auto fw = tr->tr.get_forward();
			mc->move_direction = fw;

			glm::vec3 up = { 0.f, 1.f, 0.f };
			glm::vec3 dist = -fw * 10.f + up * 7.f;
			auto cam_pos = pos + dist;

			m_game_camera->init(cam_pos, pos, up);
		}
    }
}

void
GameInstance::cleanup( )
{
}

void
GameInstance::key_pressed( input::KeyCode code, basic::int16 key )
{
	auto mc = m_player->get_component< MoveComponent >();
	auto tr = m_player->get_component< TransformComponent >();
	if (!mc || !tr)
	{
		return;
	}

	const float vel = 100.f;
	switch (code)
	{
	case input::KeyCode::A: mc->angle_speed = 10.f; break;
	case input::KeyCode::D: mc->angle_speed = -10.f; break;
	case input::KeyCode::W: mc->move_speed = vel; break;
	case input::KeyCode::S: mc->move_speed = -vel; break;
	case input::KeyCode::Space:
		{
		auto pos = tr->tr.get_position();
		pos.y += 0.1f;
		tr->tr.set_position(pos);
		}
		break;
	}
}

void
GameInstance::print_fps( ) const
{
	constexpr int BUFF_SIZE = 256;
    static basic::char_t buff[ BUFF_SIZE ];
    static basic::uint32 fps;

    if ( fps != m_engine->get_fps( ) )
    {
        fps = m_engine->get_fps( );
        if ( basic::String::format( buff, BUFF_SIZE, "fps: %u", fps ) )
        {
            m_fps_text->set_text( buff );
        }
    }

    basic::String::format( buff, BUFF_SIZE, "memory usage: %u", basic::get_memory_usage( ) );
    m_mem_text->set_text( buff );
}
