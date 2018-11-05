#pragma once

#include "basic/vector.hpp"

class Shader
{
public:
    Shader();

    void bind() const;
    
    void unbind() const;

    bool init( basic::Vector<char> vertex_data, basic::Vector<char> fragmet_data );

    basic::uint32 get_uniform( const char* name ) const;

private:
    bool compile( basic::Vector<char> data, basic::uint32 type, basic::uint32& out_id );
    
    bool link_program( basic::uint32 vshader, basic::uint32 fshader );

private:
    basic::uint32 m_shader_program;
};
