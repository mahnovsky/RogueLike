#pragma once

#include "render.hpp"
#include "render_object.hpp"

class Sprite
{
public:
    Sprite()
    {}

    void init()
    {
        VertexBuffer vb;
        vb.push( { { 0.5f, 0.5f, 0.0f }, m_color } );
        vb.push( { { 0.5f, -0.5f, 0.0f }, m_color } );
        vb.push( { {-0.5f, -0.5f, 0.0f }, m_color } );
        vb.push( { {-0.5f, 0.5f, 0.0f }, m_color } );

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

    void set_color( basic::uint8 r,
                    basic::uint8 g,
                    basic::uint8 b,
                    basic::uint8 a )
    {
        m_color = { r, g, b, a };

        m_object.update( &Sprite::update_color, this );
    }

    static void update_color( Vertex* v, void* user_data )
    {
        Sprite* sp = static_cast<Sprite*>( user_data );
        v->color = sp->m_color;
    }

private:
    RenderObject m_object;

    basic::Color m_color;
};

