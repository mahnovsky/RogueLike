#pragma once

#include "defines.hpp"
#include "resource_storage.hpp"

class BaseShader : public FileResource
{
public:
    BaseShader(ObjectManager *manager, basic::uint32 type, const char* file);
    ~BaseShader() override;

    bool load(ResourceStorage* storage) override;

    basic::uint32 get_handle() const;

    bool is_valid() const;

    static BaseShader* create(ObjectManager *manager, const char* file);

private:
    basic::uint32 m_handle;
};

class ShaderProgram : public FileResource
{
public:
    ShaderProgram(ObjectManager *manager, const char* file );
    ~ShaderProgram( ) override;

    bool load( ResourceStorage* storage ) override;

    void bind( ) const;

    void unbind( ) const;    

    basic::int32 get_uniform( const char* name ) const;

    static ShaderProgram* create(ObjectManager *manager, const char* file );

private:

    bool init( BaseShader* vertex, BaseShader* fragment );

    bool link_program( basic::uint32 vshader, basic::uint32 fshader );

private:
    basic::uint32 m_shader_program;
    BaseShader* m_vertex_shader;
    BaseShader* m_fragment_shader;
};

