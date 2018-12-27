#include "game_instance.hpp"
#include "render_common.hpp"

GameInstance::GameInstance( Engine* engine, float width, float height )
    : m_engine( engine )
    , m_texture_cache( )
    , m_game_camera( 60.f, width / height, 0.f, 1000.f )
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
    m_cam_pos = {100.f, 100.f, 100.f};
    m_cam_move_direction = glm::normalize( glm::vec3{0.f, 0.f, 0.f} - m_cam_pos );

    m_game_camera.init( m_cam_pos * 0.1f, {0.f, 0.f, 0.f}, {0.f, 1.f, 0.f} );
    m_ui_camera.init( {m_width / 2, m_height / 2, 0.f}, {}, {} );
    // m_ui_camera.init( {0.f, 0.f, 0.f}, {}, {} );

    Shader* shader = nullptr;
    ShaderCache::Handle shandle;
    if ( load_shader( m_shader_cache, "vshader.glsl", "fshader.glsl", shandle ) )
    {
        shader = m_shader_cache.get( shandle );
    }

    TextureCache::Handle handle;
    if ( shader && load_texture( m_texture_cache, "my.bmp", handle ) )
    {
        Texture* texture = m_texture_cache.get( handle );

        m_back.init( shader, texture );
        m_back.set_size( 2.f, 2.f );
        // m_back.set_color( 255, 255, 255, 20 );

        m_btn.init( shader, texture );
        m_btn.set_size( 100.f, 100.f );
        m_btn.set_position( {0.f, 0.f, 0.f} );
    }

    Shader* text_shader = nullptr;
    ShaderCache::Handle text_shandle;
    if ( load_shader( m_shader_cache, "vshader.glsl", "text_fshader.glsl", text_shandle ) )
    {
        text_shader = m_shader_cache.get( text_shandle );
    }

    if ( text_shader && m_font.init( "arial.ttf", text_shader ) )
    {
        m_text.set_font( &m_font );
        m_text.set_text( "hello world" );
        // m_text.set_scale( 0.1f );
        m_text.set_position( {200.f, 200.f, 0.f} );
    }

    Mesh m;
    TextureCache::Handle model_shandle;
    if (  // load_texture( m_texture_cache, "tex/Rock.bmp", model_shandle )
            load_mesh( "cow.obj", m ) )  //"Rck-Wtrfll_obj.obj", m ) )
    {
        // m_model.set_index_buffer( std::move( m.ib ) );
        m_model.set_vertex_buffer( std::move( m.vb ) );
        m_model.set_shader( shader );
        // m_model.set_texture( m_texture_cache.get( handle ) );
        // m_model.get_transform( )->set_scale( {10.f, 10.f, 10.f} );
        m_model.init( );
    }
}

void
GameInstance::draw( IRender* render )
{
    m_model.draw( render, &m_game_camera );
    // m_back.draw( &m_game_camera, render );
    // m_btn.draw( &m_ui_camera, render );
    // m_text.draw( render, &m_ui_camera );
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
    pos.x += ( 0.1f * delta );
    pos.y += ( 0.1f * delta );

    m_back.set_position( pos );
}

void
GameInstance::cleanup( )
{
}
