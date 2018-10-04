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

        byte red = 255;
        byte green = 0;
        byte blue = 0;
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

    void draw( IRender* render )
    {
        render->draw( &m_object );
    }

private:
    RenderObject m_object;
};


int
main( )
{
    Engine engine;

    engine.add_drawable( new Sprite() );

    return engine.run( 1024, 768 );
}
