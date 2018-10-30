#pragma once

#include "render_common.hpp"
#include "basic/pool.hpp"
#include "transform.hpp"

using vertex_update = void (*)( Vertex*, void* );

class Texture;

class IRenderObject
{
public:
    virtual void bind() const = 0;
    
    virtual void unbind() const = 0;

    virtual size_t get_element_count() const = 0;

    virtual void get_matrix( glm::mat4& out ) const = 0;

};


class RenderObject : public IRenderObject
{
public:
    RenderObject();
    ~RenderObject();

    void init();

    void update( vertex_update callback, void* user_data );

    bool is_initialized() const;

    void set_vertex_buffer( VertexBuffer buffer ); 

    void set_index_buffer( IndexBuffer buffer );

    void bind() const;
    
    void unbind() const;

    size_t get_element_count() const; 

    void get_matrix( glm::mat4& out ) const;

    // Dont save pointer, address will change
    Transform* get_transform();

    const Transform* get_transform() const;

    void set_texture( Texture* texture );

private:
    void init_vertex_buffer( );

    void init_index_buffer( );

    void bind_array_object( bool on ) const; 

    VertexBuffer m_vb;
    IndexBuffer m_ib;

    basic::uint32 m_array_object;    
    basic::uint32 m_index_object;    
    basic::uint32 m_vertex_object;    
    basic::PoolPtr<Transform> m_transform;
    Texture* m_texture;
}; 

struct Quad
{
    static void generate( VertexBuffer& out_vb, const glm::vec3& size, const glm::vec2& anchor, basic::Color c );

    static IndexBuffer::Item indices[6];
};

