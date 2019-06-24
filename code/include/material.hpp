#pragma once

#include "defines.hpp"

class Material final
{
public:
    Material( );
    Material( ShaderProgram* program, Texture* texture );
    ~Material( );

    void enable( ) const;

    void disable( ) const;

    void set_uniform( const char* name, const glm::vec2& v );

    void set_uniform( const char* name, const basic::Color& color ) const;

    void set_uniform( const char* name, const glm::mat4& mat ) const;

    const Texture* get_texture( ) const;

    void set_shader( ShaderProgram* sp );

private:
    ShaderProgram* m_shader;
    Texture* m_texture;
};
