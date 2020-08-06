#pragma once

#include "defines.hpp"

class Sprite
{
public:
    Sprite( );
    ~Sprite();

    void init( Texture* texture );

    void draw( ICamera* camera, IRender* render );

    void set_position( const glm::vec3& pos );

    void set_size( float width, float height );

    void set_anchor( float x, float y );

    void set_angle( float angle );

    float get_angle() const;

    void set_frame_name(const char* name);

    glm::vec3 get_position( ) const;

    void set_color( basic::uint8 r, basic::uint8 g, basic::uint8 b, basic::uint8 a );

private:
    void update_frame();

private:
    basic::Color m_color;
    glm::vec3 m_size;
    glm::vec2 m_anchor;
    float m_angle;
    basic::String m_frame_name;
};
