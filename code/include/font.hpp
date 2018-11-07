#pragma once

#include "basic/types.hpp"
#include "shader.hpp"
#include "sprite.hpp"
#include "texture.hpp"

namespace se
{
struct bakedchar;

class Font
{
public:
    Font( );
    ~Font( );

    bool init( const char* file, Shader* shader );

    void generate( const char* text, float height, RenderObject& out_object );

private:
    Texture m_texture;
    float m_height;
    void* m_cdata;
    Shader* m_shader;
};
}
