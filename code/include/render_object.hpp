#pragma once

#include "defines.hpp"

#include "render_common.hpp"
#include "transform.hpp"

using vertex_update = void ( * )( Vertex*, void* );

class IRenderObject
{
public:
    virtual ~IRenderObject();

    virtual void draw( IRender* render, ICamera* cam ) const = 0;

    virtual basic::uint32 get_element_count( ) const = 0;

    virtual void get_matrix( glm::mat4& out ) const = 0;
};


class RenderObject : public IRenderObject
{
public:
    RenderObject( );
    ~RenderObject( ) override;

    void init( );

    void update( vertex_update callback, void* user_data );

	void update_vertices(VertexBuffer buffer);

	void update_indices(IndexBuffer buffer);

    bool is_initialized( ) const;

    void set_vertex_buffer( VertexBuffer buffer );

    void set_index_buffer( IndexBuffer buffer );

    void draw( IRender* render, ICamera* cam ) const override;

    basic::uint32 get_element_count( ) const override;

    void get_matrix( glm::mat4& out ) const override;

    Transform* get_transform( );

    const Transform* get_transform( ) const;

    void set_texture( Texture* texture );

    void set_shader( ShaderProgram* shader );

private:
    void init_vertex_buffer( );

    void init_index_buffer( );

    void bind_array_object( bool on ) const;

private:
    VertexBuffer m_vb;
    IndexBuffer m_ib;

    basic::uint32 m_array_object;
    basic::uint32 m_index_object;
    basic::uint32 m_vertex_object;
    Transform m_transform;
    Texture* m_texture;
    ShaderProgram* m_shader;
};

struct QuadGenerator
{
    QuadGenerator( const glm::vec3& size, const glm::vec2& anchor, const basic::Color& color );

    void generate(VertexBufferT &out_vb, int offset );

    void generate( IndexBuffer& out_ib, int offset );

    static IndexBuffer::Item indices[ 6 ];

private:
    glm::vec3 m_size;
    glm::vec2 m_anchor;
    basic::Color m_color;
};
