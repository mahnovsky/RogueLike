#pragma once

#include "defines.hpp"
#include "resource_storage.hpp"
#include <GL/glew.h>

class BaseShader : public FileResource
{
public:
    GENERIC_OBJECT_IMPL(BaseShader, NS_RESOURCE_TYPE);

    BaseShader(GenericObjectManager*manager, GLenum type, const char* file);
    ~BaseShader() override;

    bool load(ResourceStorage* storage) ;

    basic::uint32 get_handle() const;

    bool is_valid() const;

    int32_t get_tag() const { return m_tag; }

    void set_tag(int32_t tag) { m_tag = tag; }

    static BaseShader* create(GenericObjectManager* manager, const char* file);

private:
    int32_t m_tag;
    basic::uint32 m_handle;
	GLenum m_type;
};

class ShaderProgram : public FileResource
{
public:
    GENERIC_OBJECT_IMPL(ShaderProgram, NS_RESOURCE_TYPE);

    ShaderProgram(GenericObjectManager *manager, const char* file );
    ~ShaderProgram( ) override;

    bool load( ResourceStorage* storage );

	basic::uint32 get_handle() const;

    void bind( ) const;

    void unbind( ) const;    

    basic::int32 get_uniform( const char* name ) const;

    static ShaderProgram* create(GenericObjectManager *manager, const char* file);

private:

    bool init( BaseShader* vertex, BaseShader* fragment );

    bool link_program( basic::uint32 vshader, basic::uint32 fshader );

private:
	static basic::uint32 m_current_shader_program;
    basic::uint32 m_shader_program;
    BaseShader* m_vertex_shader;
    BaseShader* m_fragment_shader;
};

