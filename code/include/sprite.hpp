#pragma once

#include "render.hpp"
#include "render_object.hpp"

class ICamera;

class Sprite
{
public:
    Sprite( );

    void init( Shader* shader, Texture* texture );

    void draw( ICamera* camera, IRender* render );

    void set_position( const glm::vec3& pos );

    void set_size( float width, float height );

    void set_anchor( float x, float y );

    glm::vec3 get_position( ) const;

    void set_color( basic::uint8 r, basic::uint8 g, basic::uint8 b, basic::uint8 a );

    static void update_color( Vertex* v, void* user_data );

    static void update_size( Vertex* v, void* user_data );

private:
    RenderObject m_object;
    basic::Color m_color;
    Texture* m_texture;
    Shader* m_shader;
    glm::vec3 m_size;
    glm::vec2 m_anchor;
};
