#pragma once

#include "render.hpp"
#include "render_object.hpp"

class Sprite
{
public:
    Sprite();

    void init( const char* texture_file );

    void draw( IRender* render );
 
    void set_position( const glm::vec3& pos );

    void set_size( float width, float height );

    glm::vec3 get_position() const;

    void set_color( basic::uint8 r,
                    basic::uint8 g,
                    basic::uint8 b,
                    basic::uint8 a );
   
    static void update_color( Vertex* v, void* user_data );

private:
    RenderObject m_object;
    basic::Color m_color;
    Texture m_texture;
    float m_width;
    float m_height;
};

