#include "basic/vector.hpp"
#include "basic/string.hpp"
#include "engine.hpp"
#include "render_object.hpp"

class Sprite : public IDrawable
{
public:
    Sprite()
    {}

    void init() override
    {

        basic::uint8 red = 255;
        basic::uint8 green = 0;
        basic::uint8 blue = 0;
        VertexBuffer vb;
        vb.push( { 0.5f, 0.5f, 0.0f, red, green, blue, 255 } );
        vb.push( { 0.5f, -0.5f, 0.0f, red, green, blue, 255} );
        vb.push( {-0.5f, -0.5f, 0.0f, red, green, blue, 255} );
        vb.push( {-0.5f, 0.5f, 0.0f, red, green, blue, 255} );

        m_object.set_vertex_buffer( std::move( vb ) );

        IndexBuffer ib;
        ib.push( 0 );
        ib.push( 1 );
        ib.push( 3 );
        ib.push( 1 );
        ib.push( 2 );
        ib.push( 3 );

        m_object.set_index_buffer( std::move( ib ) );

        m_object.init();
    }

    void draw( IRender* render ) override
    {
        render->draw( &m_object );
    }

private:
    RenderObject m_object;
};

static Sprite g_back;

void game_init( Engine* engine )
{
    g_back.init();
}

void game_draw( Engine* engine )
{
    IRender* render = engine->get_render();
    ASSERT( render != nullptr );

    g_back.draw( render );
}

int
main( int argv, char** argc )
{
    Engine engine( argv, argc );

    engine.set_callback( Init, &game_init );
    engine.set_callback( Draw, &game_draw );

    return engine.run( 1024, 768 );
}
