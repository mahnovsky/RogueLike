#pragma once

#include "defines.hpp"

class Material final
{
public:
    Material( ShaderProgram* program, Texture *texture );
    ~Material();

    void enable() const;

    void disable() const;

    void load_color( const char* name, const basic::Color& color ) const;

    void load_matrix( const char* name, const glm::mat4& mat ) const;

private:
    ShaderProgram* m_shader;
    Texture* m_texture;
};
