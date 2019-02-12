#pragma once

#include "object.hpp"
#include "basic/vector.hpp"
#include "cache.hpp"

enum class ShaderType
{
    VERTEX,
    FRAGMENT
};

class Shader : public Object
{
public:
    Shader();

    bool init( ShaderType type, basic::Vector<basic::uint8> data );

    basic::uint32 get_handle() const;

private:
    basic::uint32 m_handle;
    ShaderType m_type;
};

class ShaderProgram
{
public:
    ShaderProgram( );
    ~ShaderProgram( );

    void bind( ) const;

    void unbind( ) const;

    bool init(basic::Vector<basic::uint8> vertex_data, basic::Vector<basic::uint8> fragmet_data );

    basic::uint32 get_uniform( const char* name ) const;

private:
    bool compile(basic::Vector<basic::uint8> data, basic::uint32 type, basic::uint32& out_id );

    bool link_program( basic::uint32 vshader, basic::uint32 fshader );

private:
    basic::uint32 m_shader_program;
    Shader* m_vertex_shader;
    Shader* m_fragment_shader;
};

using ShaderCache = Cache< ShaderProgram >;

bool load_shader( ShaderCache& cache,
                  const char* vshader_file,
                  const char* fshader_file,
                  ShaderCache::Handle& out_handle );
